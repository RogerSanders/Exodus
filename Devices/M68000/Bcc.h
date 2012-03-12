#include "M68000Instruction.h"
#ifndef __M68000_Bcc_H__
#define __M68000_Bcc_H__
namespace M68000 {

class Bcc :public M68000Instruction
{
public:
	virtual Bcc* Clone() {return new Bcc();}
	virtual Bcc* ClonePlacement(void* buffer) {return new(buffer) Bcc();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"0110CCCC********", L"CCCC=0010-1111");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"B" + DisassembleConditionCode(conditionCode) + L"." + DisassembleSize(size), target.Disassemble(), target.DisassembleImmediateAsPCDisplacement());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---------------|-------------------------------|
//	| 0 | 1 | 1 | 0 |   CONDITION   |         8 BITS OFFSET         |
//	|---------------------------------------------------------------|
//	|            16 BITS OFFSET, IF 8 BITS OFFSET = $00             |
//	-----------------------------------------------------------------
		conditionCode = (ConditionCode)data.GetDataSegment(8, 4);

		//Bcc	<label>
		if(data.GetDataSegment(0, 8) != 0x00)
		{
			size = BITCOUNT_BYTE;
			target.BuildImmediateData(location + GetInstructionSize(), M68000Byte(data.GetDataSegment(0, 8)));
		}
		else
		{
			size = BITCOUNT_WORD;
			target.BuildImmediateData(size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(target.ExtensionSize());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long newPC;

		//Test the condition code
		bool result = ConditionCodeTrue(cpu, conditionCode);

		ExecuteTime additionalCycles;
		if(result)
		{
			//If the condition is true, branch to the target location and run the loop
			//again.
			Data offset(size);
			additionalTime += target.Read(cpu, offset, GetInstructionRegister());
			newPC = target.GetSavedPC() + M68000Long(offset.SignExtend(BITCOUNT_LONG));
			additionalCycles.Set(10, 2, 0);
		}
		else
		{
			//If the condition is false, skip the branch, and continue execution at the
			//next instruction.
			newPC = location + GetInstructionSize();
			if(size == BITCOUNT_BYTE)
			{
				additionalCycles.Set(8, 1, 0);
			}
			else
			{
				additionalCycles.Set(12, 1, 0);
			}
		}
		cpu->SetPC(newPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	Bitcount size;
	ConditionCode conditionCode;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
