#include "M68000Instruction.h"
#ifndef __M68000_MOVE_FROM_SR_H__
#define __M68000_MOVE_FROM_SR_H__
namespace M68000 {

class MOVE_from_SR :public M68000Instruction
{
public:
	virtual MOVE_from_SR* Clone() const {return new MOVE_from_SR();}
	virtual MOVE_from_SR* ClonePlacement(void* buffer) const {return new(buffer) MOVE_from_SR();}

	virtual bool Privileged() const
	{
		//Note that this instruction does not require supervisor privileges on the base M68000
		//processor. Reference: M68000 Programmers Reference Manual, table A-1, footnote 4.
		//##TODO## Run a test on the hardware to confirm this behaviour.
		return false;
	}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100000011DDDDDD", L"DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MOVE from SR";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(L"MOVE", L"SR, " + target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|

		//MOVE	SR,<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());
		if((target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect) || (target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect))
		{
			AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			AddExecuteCycleCount(target.DecodeTime());
		}
		else
		{
			AddExecuteCycleCount(ExecuteTime(6, 1, 0));
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Word result;

		//Perform the operation
		result = cpu->GetSR();
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

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
