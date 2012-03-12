#include "M68000Instruction.h"
#ifndef __M68000_UNLK_H__
#define __M68000_UNLK_H__
namespace M68000 {

class UNLK :public M68000Instruction
{
public:
	virtual UNLK* Clone() {return new UNLK();}
	virtual UNLK* ClonePlacement(void* buffer) {return new(buffer) UNLK();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"0100111001011***", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"UNLK", source.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 0 | 1 | 1 | REGISTER  |
//	-----------------------------------------------------------------
		source.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(0, 3));
		stackPointer.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		target.BuildAddressPostinc(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		AddExecuteCycleCount(ExecuteTime(12, 3, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long originalSP;
		M68000Long originalData;

		//Read source register, and write it into SP
		additionalTime += source.Read(cpu, originalSP, GetInstructionRegister());
		additionalTime += stackPointer.Write(cpu, originalSP, GetInstructionRegister());

		//Load original register contents
		additionalTime += target.Read(cpu, originalData, GetInstructionRegister());
		additionalTime += source.Write(cpu, originalData, GetInstructionRegister());

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	EffectiveAddress stackPointer;
};

} //Close namespace M68000
#endif
