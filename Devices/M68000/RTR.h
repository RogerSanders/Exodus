#include "M68000Instruction.h"
#ifndef __M68000_RTR_H__
#define __M68000_RTR_H__
namespace M68000 {

class RTR :public M68000Instruction
{
public:
	virtual RTR* Clone() const {return new RTR();}
	virtual RTR* ClonePlacement(void* buffer) const {return new(buffer) RTR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100111001110111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RTR";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 | 1 | 1 |
//	-----------------------------------------------------------------
		target.BuildAddressPostinc(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		AddExecuteCycleCount(ExecuteTime(20, 5, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Word originalSR;
		M68000Long originalPC;

		//Perform the operation
		additionalTime += target.Read(cpu, originalSR, GetInstructionRegister());
		additionalTime += target.Read(cpu, originalPC, GetInstructionRegister());
		cpu->PopCallStack(originalPC.GetData());
		cpu->SetCCR(M68000Byte(originalSR.Convert(BITCOUNT_BYTE)));
		cpu->SetPC(originalPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetResultantPCLocations(std::set<unsigned int>& resultantPCLocations, bool& undeterminedResultantPCLocation) const
	{
		//Since this opcode returns to a previous address saved on the stack, and we don't
		//care about return opcodes when calculating resultant PC locations, don't return
		//any resultant PC locations from executing this opcode.
		undeterminedResultantPCLocation = false;
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{ }

private:
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
