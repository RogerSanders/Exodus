#include "M68000Instruction.h"
#ifndef __M68000_NOP_H__
#define __M68000_NOP_H__
namespace M68000 {

class NOP :public M68000Instruction
{
public:
	virtual NOP* Clone() const {return new NOP();}
	virtual NOP* ClonePlacement(void* buffer) const {return new(buffer) NOP();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100111001110001", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"NOP";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 0 | 1 |
//	-----------------------------------------------------------------
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount();
	}
};

} //Close namespace M68000
#endif
