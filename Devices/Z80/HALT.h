#include "Z80Instruction.h"
#ifndef __Z80_HALT_H__
#define __Z80_HALT_H__
namespace Z80 {

class HALT :public Z80Instruction
{
public:
	virtual HALT* Clone() {return new HALT();}
	virtual HALT* ClonePlacement(void* buffer) {return new(buffer) HALT();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"01110110", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"HALT", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		//Perform the operation
		cpu->SetProcessorStoppedState(true);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount();
	}
};

} //Close namespace Z80
#endif
