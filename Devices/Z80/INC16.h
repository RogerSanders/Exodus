#include "Z80Instruction.h"
#ifndef __Z80_INC16_H__
#define __Z80_INC16_H__
namespace Z80 {

class INC16 :public Z80Instruction
{
public:
	virtual INC16* Clone() {return new INC16();}
	virtual INC16* ClonePlacement(void* buffer) {return new(buffer) INC16();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"00**0011", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"INC", target.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		//INC ss		00ss0011
		//INC IX		11011101 00ss0011
		//INC IY		11111101 00ss0011
		target.Decode16BitRegister(data.GetDataSegment(4, 2));

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(6);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word op1;
		Z80Word result;

		//Perform the operation
		additionalTime += target.Read(cpu, location, op1);
		result = op1 + 1;
		additionalTime += target.Write(cpu, location, result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
