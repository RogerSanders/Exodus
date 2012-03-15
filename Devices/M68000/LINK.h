#include "M68000Instruction.h"
#ifndef __M68000_LINK_H__
#define __M68000_LINK_H__
namespace M68000 {

class LINK :public M68000Instruction
{
public:
	virtual LINK* Clone() {return new LINK();}
	virtual LINK* ClonePlacement(void* buffer) {return new(buffer) LINK();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"0100111001010***", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"LINK." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 0 | 1 | 0 | REGISTER  |
//	|---------------------------------------------------------------|
//	|                        16 BITS OFFSET (d)                     |
//	-----------------------------------------------------------------
		size = BITCOUNT_WORD;
		source.BuildAddressDirect(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
		stackPointer.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		target.BuildAddressPredec(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		offset.BuildImmediateData(size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(offset.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(16, 2, 2));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long currentReg;
		M68000Long currentStack;
		M68000Long offsetData;

		//Read source register, and write it to the stack
		additionalTime += source.Read(cpu, currentReg, GetInstructionRegister());
		additionalTime += target.Write(cpu, currentReg, GetInstructionRegister());

		//Write stack address into source register
		additionalTime += stackPointer.Read(cpu, currentStack, GetInstructionRegister());
		additionalTime += source.Write(cpu, currentStack, GetInstructionRegister());

		//Add offset to stack address
		additionalTime += offset.Read(cpu, offsetData, GetInstructionRegister());
		currentStack += offsetData;
		additionalTime += stackPointer.Write(cpu, currentStack, GetInstructionRegister());

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	EffectiveAddress offset;
	EffectiveAddress stackPointer;
	Bitcount size;
};

} //Close namespace M68000
#endif
