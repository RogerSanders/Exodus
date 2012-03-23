#include "M68000Instruction.h"
#ifndef __M68000_BRA_H__
#define __M68000_BRA_H__
namespace M68000 {

class BRA :public M68000Instruction
{
public:
	virtual BRA* Clone() const {return new BRA();}
	virtual BRA* ClonePlacement(void* buffer) const {return new(buffer) BRA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01100000********", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"BRA";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(size), target.Disassemble(), target.DisassembleImmediateAsPCDisplacement());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|-------------------------------|
//	| 0 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |         8 BITS OFFSET         |
//	|---------------------------------------------------------------|
//	|            16 BITS OFFSET, IF 8 BITS OFFSET = $00             |
//	-----------------------------------------------------------------

		//BRA	<label>
		if(data.GetDataSegment(0, 8) != 0)
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
		AddExecuteCycleCount(ExecuteTime(10, 2, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long newPC;

		//Return the execution time
		Data offset(size);
		additionalTime += target.Read(cpu, offset, GetInstructionRegister());
		newPC = target.GetSavedPC() + M68000Long(offset.SignExtend(BITCOUNT_LONG));
		cpu->SetPC(newPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

private:
	Bitcount size;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
