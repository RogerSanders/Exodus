#include "Z80Instruction.h"
#ifndef __Z80_EI_H__
#define __Z80_EI_H__
namespace Z80 {

class EI :public Z80Instruction
{
public:
	virtual EI* Clone() const {return new EI();}
	virtual EI* ClonePlacement(void* buffer) const {return new(buffer) EI();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"11111011", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"EI";
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
		cpu->SetIFF1(true);
		cpu->SetIFF2(true);

		//Flag that an EI opcode has just been executed. See notes in Z80::ExecuteCycle.
		cpu->SetMaskInterruptsNextOpcode(true);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount();
	}
};

} //Close namespace Z80
#endif
