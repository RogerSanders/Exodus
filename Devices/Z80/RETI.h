#include "Z80Instruction.h"
#ifndef __Z80_RETI_H__
#define __Z80_RETI_H__
namespace Z80 {

class RETI :public Z80Instruction
{
public:
	virtual RETI* Clone() const {return new RETI();}
	virtual RETI* ClonePlacement(void* buffer) const {return new(buffer) RETI();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01001101", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RETI";
	}

	virtual Disassembly Z80Disassemble() const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetMode(EffectiveAddress::MODE_SP_POSTINC);
		AddExecuteCycleCount(10);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word oldPC;

		//Perform the operation
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
