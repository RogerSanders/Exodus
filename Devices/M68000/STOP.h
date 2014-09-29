#include "M68000Instruction.h"
#ifndef __M68000_STOP_H__
#define __M68000_STOP_H__
namespace M68000 {

class STOP :public M68000Instruction
{
public:
	virtual STOP* Clone() const {return new STOP();}
	virtual STOP* ClonePlacement(void* buffer) const {return new(buffer) STOP();}

	virtual bool Privileged() const
	{
		return true;
	}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100111001110010", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"STOP";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), source.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 0 |
//	|---------------------------------------------------------------|
//	|                    16 BITS IMMEDIATE DATA                     |
//	-----------------------------------------------------------------
		source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(4, 0, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Word newSR;

		//Perform the operation
		additionalTime += source.Read(cpu, newSR, GetInstructionRegister());
		cpu->SetSR(newSR);
		//##TODO## We had some code here to generate a privilege violation if the new
		//value for SR didn't have the supervisor bit set. Do some more research, and see
		//if we can find some supporting evidence for this behaviour.
		cpu->SetProcessorState(M68000::State::Stopped);

		//Return the execution time
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
