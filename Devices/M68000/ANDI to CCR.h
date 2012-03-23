#include "M68000Instruction.h"
#ifndef __M68000_ANDI_TO_CCR_H__
#define __M68000_ANDI_TO_CCR_H__
namespace M68000 {

class ANDI_to_CCR :public M68000Instruction
{
public:
	virtual ANDI_to_CCR* Clone() const {return new ANDI_to_CCR();}
	virtual ANDI_to_CCR* ClonePlacement(void* buffer) const {return new(buffer) ANDI_to_CCR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0000001000111100", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ANDI to CCR";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(L"ANDI", source.Disassemble() + L", CCR");
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 1 | 1 | 1 | 1 | 0 | 0 |
//	|---|---|---|---|---|---|---|---|-------------------------------|
//	| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |     8 BITS IMMEDIATE DATA     |
//	-----------------------------------------------------------------

		//ANDI	#<data>,CCR
		source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(20, 3, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Byte op1;
		M68000Byte op2;
		M68000Byte result;

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		op2 = cpu->GetCCR();
		result = op1 & op2;
		cpu->SetCCR(result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
};

} //Close namespace M68000
#endif
