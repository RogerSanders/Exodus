#include "Z80Instruction.h"
#ifndef __Z80_DI_H__
#define __Z80_DI_H__
namespace Z80 {

class DI :public Z80Instruction
{
public:
	virtual DI* Clone() const {return new DI();}
	virtual DI* ClonePlacement(void* buffer) const {return new(buffer) DI();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"11110011", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"DI";
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
