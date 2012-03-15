#include "Z80Instruction.h"
#ifndef __Z80_DEC16_H__
#define __Z80_DEC16_H__
namespace Z80 {

class DEC16 :public Z80Instruction
{
public:
	virtual DEC16* Clone() {return new DEC16();}
	virtual DEC16* ClonePlacement(void* buffer) {return new(buffer) DEC16();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"00**1011", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"DEC", target.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		//DEC ss		00ss1011
		//DEC IX		11011101 00ss1011
		//DEC IY		11111101 00ss1011
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
		result = op1 - 1;
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
