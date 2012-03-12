#include "Z80Instruction.h"
#ifndef __Z80_DI_H__
#define __Z80_DI_H__
namespace Z80 {

class DI :public Z80Instruction
{
public:
	virtual DI* Clone() {return new DI();}
	virtual DI* ClonePlacement(void* buffer) {return new(buffer) DI();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"11110011", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"DI", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		//Perform the operation
		cpu->SetIFF1(false);
		cpu->SetIFF2(false);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount();
	}
};

} //Close namespace Z80
#endif
