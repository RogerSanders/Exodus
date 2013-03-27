#include "M68000Instruction.h"
#ifndef __M68000_MOVEQ_H__
#define __M68000_MOVEQ_H__
namespace M68000 {

class MOVEQ :public M68000Instruction
{
public:
	virtual MOVEQ* Clone() const {return new MOVEQ();}
	virtual MOVEQ* ClonePlacement(void* buffer) const {return new(buffer) MOVEQ();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0111***0********", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MOVEQ";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), source.Disassemble(labelSettings) + L", " + target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------------------------------|
//	| 0 | 1 | 1 | 1 | REGISTER  | 0 |        IMMEDIATE DATA         |
//	-----------------------------------------------------------------

		source.BuildImmediateData(location + GetInstructionSize(), M68000Byte(data.GetDataSegment(0, 8)), true);
		target.BuildDataDirect(BITCOUNT_BYTE, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Byte result;

		//Perform the operation
		additionalTime += source.Read(cpu, result, GetInstructionRegister());
		additionalTime += target.Write(cpu, result.SignExtend(BITCOUNT_LONG), GetInstructionRegister());

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		cpu->SetC(false);

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
