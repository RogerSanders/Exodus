#include "M68000Instruction.h"
#ifndef __M68000_CHK_H__
#define __M68000_CHK_H__
namespace M68000 {

class CHK :public M68000Instruction
{
public:
	virtual CHK* Clone() const {return new CHK();}
	virtual CHK* ClonePlacement(void* buffer) const {return new(buffer) CHK();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100***CC0DDDDDD", L"CC=11 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"CHK";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|-------|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 |  REGISTER |  SIZE | 0 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		switch (data.GetDataSegment(7, 2))
		{
		case 3:	// 11
			_size = BITCOUNT_WORD;
			break;
		}

		_source.BuildDataDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_target.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
		AddExecuteCycleCount(_target.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(_size);
		Data op2(_size);
		ExecuteTime exceptionTime;

		// Perform the operation
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.Read(cpu, op2, GetInstructionRegister());

		//##NOTE## Although these flags are officially undefined, their behaviour is
		// described in "68000 Undocumented Behavior Notes" by Bart Trzynadlowski.
		//##TODO## The state of V and C is not a definite. Run a full battery of tests
		// to confirm the state of V and C for all possible input values. Also test the
		// result of N when an exception is not triggered.
		cpu->SetZ(op1.Negative());
		cpu->SetV(false);
		cpu->SetC(false);

		cpu->SetPC(location + GetInstructionSize());
		if (op1.Negative())	// If op1 < 0
		{
			if (!cpu->ExceptionDisabled(M68000::Exceptions::CHKInstruction))
			{
				cpu->SetN(true);
				cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
				exceptionTime = cpu->ProcessException(M68000::Exceptions::CHKInstruction);
			}
		}
		else if (op2.Negative() || (op1.GetData() > op2.GetData()))	// If op2 > op1
		{
			if (!cpu->ExceptionDisabled(M68000::Exceptions::CHKInstruction))
			{
				cpu->SetN(false);
				cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
				exceptionTime = cpu->ProcessException(M68000::Exceptions::CHKInstruction);
			}
		}
		else
		{
			//##TODO## N is officially undefined here. Run some tests to determine its
			// real behaviour.
		}

		// Return the execution time
		return GetExecuteCycleCount(additionalTime) + exceptionTime;
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		_source.AddLabelTargetsToSet(labelTargetLocations);
		_target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
	Bitcount _size;
};

} // Close namespace M68000
#endif
