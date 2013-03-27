#include "Z80Instruction.h"
#ifndef __Z80_CCF_H__
#define __Z80_CCF_H__
namespace Z80 {

class CCF :public Z80Instruction
{
public:
	virtual CCF* Clone() const {return new CCF();}
	virtual CCF* ClonePlacement(void* buffer) const {return new(buffer) CCF();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00111111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"CCF";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
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
