#include "M68000Instruction.h"
#ifndef __M68000_LINK_H__
#define __M68000_LINK_H__
namespace M68000 {

class LINK :public M68000Instruction
{
public:
	virtual LINK* Clone() const {return new LINK();}
	virtual LINK* ClonePlacement(void* buffer) const {return new(buffer) LINK();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100111001010***", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LINK";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _offset.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 0 | 1 | 0 | REGISTER  |
//	|---------------------------------------------------------------|
//	|                        16 BITS OFFSET (d)                     |
//	-----------------------------------------------------------------
		_size = BITCOUNT_WORD;
		_source.BuildAddressDirect(_size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
		_stackPointer.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		_target.BuildAddressPredec(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		_offset.BuildImmediateData(_size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister(), true);
		AddInstructionSize(_offset.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(16, 2, 2));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long currentReg;
		M68000Long currentStack;
		M68000Long offsetData;

		// Read _source register, and write it to the stack
		additionalTime += _source.Read(cpu, currentReg, GetInstructionRegister());
		additionalTime += _target.Write(cpu, currentReg, GetInstructionRegister());

		// Write stack address into _source register
		additionalTime += _stackPointer.Read(cpu, currentStack, GetInstructionRegister());
		additionalTime += _source.Write(cpu, currentStack, GetInstructionRegister());

		// Add offset to stack address
		additionalTime += _offset.Read(cpu, offsetData, GetInstructionRegister());
		currentStack += offsetData;
		additionalTime += _stackPointer.Write(cpu, currentStack, GetInstructionRegister());

		// Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{ }

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
	EffectiveAddress _offset;
	EffectiveAddress _stackPointer;
	Bitcount _size;
};

} // Close namespace M68000
#endif
