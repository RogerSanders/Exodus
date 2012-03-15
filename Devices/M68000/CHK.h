#include "M68000Instruction.h"
#ifndef __M68000_CHK_H__
#define __M68000_CHK_H__
namespace M68000 {

class CHK :public M68000Instruction
{
public:
	virtual CHK* Clone() {return new CHK();}
	virtual CHK* ClonePlacement(void* buffer) {return new(buffer) CHK();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"0100***CC0DDDDDD", L"CC=11 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"CHK." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|-------|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 |  REGISTER |  SIZE | 0 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		switch(data.GetDataSegment(7, 2))
		{
		case 3:	//11
			size = BITCOUNT_WORD;
			break;
		}

		source.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
		AddExecuteCycleCount(target.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size);
		Data op2(size);
		ExecuteTime exceptionTime;

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.Read(cpu, op2, GetInstructionRegister());

		//##NOTE## Although these flags are officially undefined, their behaviour is
		//described in "68000 Undocumented Behavior Notes" by Bart Trzynadlowski.
		//##TODO## The state of V and C is not a definite. Run a full battery of tests
		//to confirm the state of V and C for all possible input values. Also test the
		//result of N when an exception is not triggered.
		cpu->SetZ(op1.Negative());
		cpu->SetV(false);
		cpu->SetC(false);

		cpu->SetPC(location + GetInstructionSize());
		if(op1.Negative())	//If op1 < 0
		{
			if(!cpu->ExceptionDisabled(M68000::EX_CHK_INSTRUCTION))
			{
				cpu->SetN(true);
				cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
				exceptionTime = cpu->ProcessException(M68000::EX_CHK_INSTRUCTION);
			}
		}
		else if(op2.Negative() || (op1.GetData() > op2.GetData()))	//If op2 > op1
		{
			if(!cpu->ExceptionDisabled(M68000::EX_CHK_INSTRUCTION))
			{
				cpu->SetN(false);
				cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
				exceptionTime = cpu->ProcessException(M68000::EX_CHK_INSTRUCTION);
			}
		}
		else
		{
			//##TODO## N is officially undefined here. Run some tests to determine its
			//real behaviour.
		}

		//Return the execution time
		return GetExecuteCycleCount(additionalTime) + exceptionTime;
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
