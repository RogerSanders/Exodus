#include "M68000Instruction.h"
#ifndef __M68000_ANDI_TO_SR_H__
#define __M68000_ANDI_TO_SR_H__
namespace M68000 {

class ANDI_to_SR :public M68000Instruction
{
public:
	virtual ANDI_to_SR* Clone() const {return new ANDI_to_SR();}
	virtual ANDI_to_SR* ClonePlacement(void* buffer) const {return new(buffer) ANDI_to_SR();}

	virtual bool Privileged() const
	{
		return true;
	}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0000001001111100", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ANDI to SR";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(L"ANDI", source.Disassemble(labelSettings) + L", SR");
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 1 | 1 | 1 | 1 | 1 | 0 | 0 |
//	|---------------------------------------------------------------|
//	|                     16 BITS IMMEDIATE DATA                    |
//	-----------------------------------------------------------------

		//ANDI	#<data>,SR
		source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(20, 3, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Word op1;
		M68000Word op2;
		M68000Word result;

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		op2 = cpu->GetSR();
		result = op1 & op2;
		cpu->SetSR(result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		source.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress source;
};

} //Close namespace M68000
#endif
