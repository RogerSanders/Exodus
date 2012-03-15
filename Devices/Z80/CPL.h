#include "Z80Instruction.h"
#ifndef __Z80_CPL_H__
#define __Z80_CPL_H__
namespace Z80 {

class CPL :public Z80Instruction
{
public:
	virtual CPL* Clone() {return new CPL();}
	virtual CPL* ClonePlacement(void* buffer) {return new(buffer) CPL();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"00101111", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"CPL", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		target.SetMode(EffectiveAddress::MODE_A);
		AddExecuteCycleCount(4);

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		//Perform the operation
		additionalTime += target.Read(cpu, location, op1);
		result = ~op1;
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(true);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagN(true);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
