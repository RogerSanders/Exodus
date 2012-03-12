#include "M68000Instruction.h"
#ifndef __M68000_ADDI_H__
#define __M68000_ADDI_H__
namespace M68000 {

class ADDI :public M68000Instruction
{
public:
	virtual ADDI* Clone() {return new ADDI();}
	virtual ADDI* ClonePlacement(void* buffer) {return new(buffer) ADDI();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"00000110CCDDDDDD", L"CC=00-10 DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"ADDI." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//                                          |----------<ea>---------|
//	----------------------------------------=========================
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|-------|-----------|-----------|
//	| 0 | 0 | 0 | 0 | 0 | 1 | 1 | 0 | SIZE  |   MODE    |  REGISTER |
//	|---------------------------------------------------------------|
//	| 16 BITS DATA (with last Byte) |          8 BITS DATA          |
//	|---------------------------------------------------------------|
//	|             32 BITS DATA (included last Word)                 |
//	-----------------------------------------------------------------
		switch(data.GetDataSegment(6, 2))
		{
		case 0:	//00
			size = BITCOUNT_BYTE;
			break;
		case 1:	//01
			size = BITCOUNT_WORD;
			break;
		case 2:	//10
			size = BITCOUNT_LONG;
			break;
		}

		//ADDI	#<data>,<ea>
		source.BuildImmediateData(size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());

		if(target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT)
		{
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 2, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(16, 3, 0));
			}
		}
		else
		{
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(12, 2, 1));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(20, 3, 2));
			}
			AddExecuteCycleCount(target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size);
		Data op2(size);
		Data result(size);

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		result = op2 + op1;
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		bool carry = (op1.MSB() && op2.MSB()) || (!result.MSB() && (op1.MSB() || op2.MSB())) ;
		bool overflow = (op1.MSB() == op2.MSB()) && (result.MSB() != (op1.MSB() && op2.MSB()));
		cpu->SetX(carry);
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(overflow);
		cpu->SetC(carry);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
