#include "M68000Instruction.h"
#ifndef __M68000_BSR_H__
#define __M68000_BSR_H__
namespace M68000 {

class BSR :public M68000Instruction
{
public:
	virtual BSR* Clone() {return new BSR();}
	virtual BSR* ClonePlacement(void* buffer) {return new(buffer) BSR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"01100001********", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"BSR." + DisassembleSize(size), source.Disassemble(), source.DisassembleImmediateAsPCDisplacement());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|-------------------------------|
//	| 0 | 1 | 1 | 0 | 0 | 0 | 0 | 1 |         8 BITS OFFSET         |
//	|---------------------------------------------------------------|
//	|            16 BITS OFFSET, IF 8 BITS OFFSET = $00             |
//	-----------------------------------------------------------------

		//BSR	<label>
		target.BuildAddressPredec(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);

		if(data.GetDataSegment(0, 8) != 0)
		{
			size = BITCOUNT_BYTE;
			source.BuildImmediateData(location + GetInstructionSize(), M68000Byte(data.GetDataSegment(0, 8)));
		}
		else
		{
			size = BITCOUNT_WORD;
			source.BuildImmediateData(size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(source.ExtensionSize());
		}
		AddExecuteCycleCount(ExecuteTime(18, 2, 2));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data offset(size);
		M68000Long newPC;
		M68000Long returnAddress;

		//Perform the operation
		additionalTime += source.Read(cpu, offset, GetInstructionRegister());
		newPC = source.GetSavedPC() + M68000Long(offset.SignExtend(BITCOUNT_LONG));
		returnAddress = location + GetInstructionSize();
		target.Write(cpu, returnAddress, GetInstructionRegister());
		cpu->PushCallStack(cpu->GetPC().GetData(), newPC.GetData(), returnAddress.GetData(), L"BSR");

		//Return the execution time
		cpu->SetPC(newPC);
		return GetExecuteCycleCount(additionalTime);
	}

private:
	Bitcount size;
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
