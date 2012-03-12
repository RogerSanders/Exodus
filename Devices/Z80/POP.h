#include "Z80Instruction.h"
#ifndef __Z80_POP_H__
#define __Z80_POP_H__
namespace Z80 {

class POP :public Z80Instruction
{
public:
	virtual POP* Clone() {return new POP();}
	virtual POP* ClonePlacement(void* buffer) {return new(buffer) POP();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"11**0001", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"POP", target.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		//POP qq		11qq0001
		//POP IX		11011101 11100001
		//POP IY		11111101 11100001
		source.SetMode(EffectiveAddress::MODE_SP_POSTINC);
		switch(data.GetDataSegment(4, 2))
		{
		case 0:
			target.SetMode(EffectiveAddress::MODE_BC);
			break;
		case 1:
			target.SetMode(EffectiveAddress::MODE_DE);
			break;
		case 2:
			target.SetMode(EffectiveAddress::MODE_HL);
			break;
		case 3:
			target.SetMode(EffectiveAddress::MODE_AF);
			break;
		}

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(10);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word result;

		//Perform the operation
		additionalTime += source.Read(cpu, location, result);
		additionalTime += target.Write(cpu, location, result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
