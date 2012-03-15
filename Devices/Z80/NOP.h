#include "Z80Instruction.h"
#ifndef __Z80_NOP_H__
#define __Z80_NOP_H__
namespace Z80 {

class NOP :public Z80Instruction
{
public:
	virtual NOP* Clone() {return new NOP();}
	virtual NOP* ClonePlacement(void* buffer) {return new(buffer) NOP();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"00000000", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"NOP", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount();
	}
};

} //Close namespace Z80
#endif
