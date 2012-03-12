#include "Z80Instruction.h"
#ifndef __Z80_RLA_H__
#define __Z80_RLA_H__
namespace Z80 {

class RLA :public Z80Instruction
{
public:
	virtual RLA* Clone() {return new RLA();}
	virtual RLA* ClonePlacement(void* buffer) {return new(buffer) RLA();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"00010111", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"RLA", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_A);
		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		//Perform the operation
		additionalTime += target.Read(cpu, location, op1);
		result = (op1 << 1);
		result.SetBit(0, cpu->GetFlagC());
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagN(false);
		cpu->SetFlagC(op1.MSB());

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
