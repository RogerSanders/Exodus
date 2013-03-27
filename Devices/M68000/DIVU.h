#include "M68000Instruction.h"
#ifndef __M68000_DIVU_H__
#define __M68000_DIVU_H__
namespace M68000 {

class DIVU :public M68000Instruction
{
public:
	virtual DIVU* Clone() const {return new DIVU();}
	virtual DIVU* ClonePlacement(void* buffer) const {return new(buffer) DIVU();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"1000***011DDDDDD", L"DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"DIVU";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(BITCOUNT_WORD), source.Disassemble(labelSettings) + L", " + target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|---|---|-----------|-----------|
//	| 1 | 0 | 0 | 0 | REGISTER  | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|

		//DIVU.W	<ea>,Dn     32/16 -> 16r:16q
		target.BuildDataDirect(BITCOUNT_WORD, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());

		//##NOTE## This is apparently a worst case execution time. According to the
		//M68000 Users Manual, this can vary by a little under 10% in a best case
		//scenario, but I haven't found any reference on how to calculate the timing.
		AddExecuteCycleCount(ExecuteTime(140, 1, 0));
		AddExecuteCycleCount(source.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Word temp1;
		M68000Long op1;
		M68000Long op2;
		M68000Word quotient;
		M68000Word remainder;
		M68000Long result;

		//Read the source data
		additionalTime += source.Read(cpu, temp1, GetInstructionRegister());
		op1 = M68000Long(temp1.Convert(BITCOUNT_LONG));
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());

		//Check for divide by zero
		if(op1 == 0)
		{
			//UNDEFINED N
			//UNDEFINED Z
			//UNDEFINED V
			if(cpu->ExceptionDisabled(M68000::EX_ZERO_DIVIDE))
			{
				return GetExecuteCycleCount(additionalTime);
			}
			else
			{
				cpu->SetPC(location + GetInstructionSize());
				cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
				return GetExecuteCycleCount(additionalTime) + cpu->ProcessException(M68000::EX_ZERO_DIVIDE);
			}
		}

		//Perform a test run of the operation to check for an overflow. Our source data
		//is 32-bit, and the final quotient is only 16-bit, so an overflow can occur if
		//the quotient after division is still larger than can fit into a 16-bit target.
		//To test for this, we perform the division and test if the result is larger than
		//the maximum number which can be stored in the result.
		if((op2 / op1) <= quotient.GetMaxValue())
		{
			//The operation didn't overflow. Commit the results.
			quotient = M68000Word(op2 / op1);
			remainder = M68000Word(op2 % op1);
			result.SetUpperBits(remainder);
			result.SetLowerBits(quotient);
			additionalTime += target.Write(cpu, result, GetInstructionRegister());

			//Set the flag results
			//##TODO## Check handling of this flag. Official M68000 programmer's manual
			//says it should be set if quotient is negative, but this is an unsigned
			//divide. It's probably correct, and the MSB of the source sets the N flag,
			//but do a test on real hardware and see if a quotient of 0x8000 in an
			//unsigned divide sets the N flag.
			cpu->SetN(quotient.Negative());
			cpu->SetZ(quotient.Zero());
			cpu->SetV(false);
			cpu->SetC(false);
		}
		else
		{
			//An overflow has occurred. In this case, the result of the operation is not
			//saved, and the flag results differ.

			//Set the flag results
			//UNDEFINED N
			//UNDEFINED Z
			cpu->SetV(true);
			cpu->SetC(false);
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		source.AddLabelTargetsToSet(labelTargetLocations);
		target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
