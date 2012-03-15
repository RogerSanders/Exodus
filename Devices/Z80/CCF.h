#include "Z80Instruction.h"
#ifndef __Z80_CCF_H__
#define __Z80_CCF_H__
namespace Z80 {

class CCF :public Z80Instruction
{
public:
	virtual CCF* Clone() {return new CCF();}
	virtual CCF* ClonePlacement(void* buffer) {return new(buffer) CCF();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"00111111", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"CCF", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		//Set the flag results
		cpu->SetFlagH(cpu->GetFlagC());
		cpu->SetFlagN(false);
		cpu->SetFlagC(!cpu->GetFlagC());

		Z80Byte currentRegA;
		cpu->GetA(currentRegA);
		cpu->SetFlagY(currentRegA.GetBit(5));
		cpu->SetFlagX(currentRegA.GetBit(3));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount();
	}
};

} //Close namespace Z80
#endif
