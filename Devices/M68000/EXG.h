#include "M68000Instruction.h"
#ifndef __M68000_EXG_H__
#define __M68000_EXG_H__
namespace M68000 {

class EXG :public M68000Instruction
{
public:
	virtual EXG* Clone() const {return new EXG();}
	virtual EXG* ClonePlacement(void* buffer) const {return new(buffer) EXG();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"1100***1DDDDD***", L"DDDDD=01000,01001,10001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"EXG";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), source.Disassemble(labelSettings) + L", " + target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------------------|-----------|
//	| 1 | 1 | 0 | 0 |Rx REGISTER| 1 |       OP-MODE     |Ry REGISTER|
//	-----------------------------------------------------------------
		switch(data.GetDataSegment(3, 5))
		{
		case 0x8:	//01000->Exchange between data registers.
			source.BuildDataDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			target.BuildDataDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			break;
		case 0x9:	//01001->Exchange between address registers.
			source.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			target.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			break;
		case 0x11:	//10001->Exchange between data and address registers.
			source.BuildDataDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			target.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			break;
		}
		AddExecuteCycleCount(ExecuteTime(6, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long op1;
		M68000Long op2;

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.Read(cpu, op2, GetInstructionRegister());
		unsigned int sourceReadFromAddress;
		bool sourceIsUnmodifiedFromAddress = source.IsTargetUnmodifiedFromMemoryRead(cpu, BITCOUNT_LONG, sourceReadFromAddress);
		unsigned int sourceReadFromAddressSize = source.GetTargetOriginalMemoryReadSize(cpu, BITCOUNT_LONG);
		unsigned int targetReadFromAddress;
		bool targetIsUnmodifiedFromAddress = target.IsTargetUnmodifiedFromMemoryRead(cpu, BITCOUNT_LONG, targetReadFromAddress);
		unsigned int targetReadFromAddressSize = target.GetTargetOriginalMemoryReadSize(cpu, BITCOUNT_LONG);
		additionalTime += source.Write(cpu, op2, GetInstructionRegister(), false, false, targetIsUnmodifiedFromAddress, targetReadFromAddress, targetReadFromAddressSize);
		additionalTime += target.Write(cpu, op1, GetInstructionRegister(), false, false, sourceIsUnmodifiedFromAddress, sourceReadFromAddress, sourceReadFromAddressSize);

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
