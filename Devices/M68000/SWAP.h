#include "M68000Instruction.h"
#ifndef __M68000_SWAP_H__
#define __M68000_SWAP_H__
namespace M68000 {

class SWAP :public M68000Instruction
{
public:
	virtual SWAP* Clone() const {return new SWAP();}
	virtual SWAP* ClonePlacement(void* buffer) const {return new(buffer) SWAP();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100100001000***", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"SWAP";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | REGISTER  |
//	-----------------------------------------------------------------
		target.BuildDataDirect(BITCOUNT_WORD, location + GetInstructionSize(), data.GetDataSegment(0, 3));
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long op1;
		M68000Long result;

		//Perform the operation
		additionalTime += target.Read(cpu, op1, GetInstructionRegister());
		result.SetLowerBits(BITCOUNT_WORD, op1.GetUpperBits(BITCOUNT_WORD));
		result.SetUpperBits(BITCOUNT_WORD, op1.GetLowerBits(BITCOUNT_WORD));
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		cpu->SetC(false);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
