#include "Z80Instruction.h"
#ifndef __Z80_RETN_H__
#define __Z80_RETN_H__
namespace Z80 {

class RETN :public Z80Instruction
{
public:
	virtual RETN* Clone() const {return new RETN();}
	virtual RETN* ClonePlacement(void* buffer) const {return new(buffer) RETN();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01000101", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RETN";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetMode(EffectiveAddress::Mode::SPPostInc);
		AddExecuteCycleCount(10);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;

		//Perform the operation
		cpu->SetIFF1(cpu->GetIFF2());
		Z80Word oldPC;
		additionalTime += source.Read(cpu, location + GetInstructionSize(), oldPC);
		cpu->PopCallStack(oldPC.GetData());
		cpu->SetPC(oldPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
};

} //Close namespace Z80
#endif
