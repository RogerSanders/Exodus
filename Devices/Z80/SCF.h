#include "Z80Instruction.h"
#ifndef __Z80_SCF_H__
#define __Z80_SCF_H__
namespace Z80 {

class SCF :public Z80Instruction
{
public:
	virtual SCF* Clone() const {return new SCF();}
	virtual SCF* ClonePlacement(void* buffer) const {return new(buffer) SCF();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00110111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"SCF";
	}

	virtual Disassembly Z80Disassemble() const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		//Set the flag results
		cpu->SetFlagH(false);
		cpu->SetFlagN(false);
		cpu->SetFlagC(true);

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
