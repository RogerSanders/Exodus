#include "Z80Instruction.h"
#ifndef __Z80_LD16ED_H__
#define __Z80_LD16ED_H__
namespace Z80 {

class LD16ED :public Z80Instruction
{
public:
	virtual LD16ED* Clone() {return new LD16ED();}
	virtual LD16ED* ClonePlacement(void* buffer) {return new(buffer) LD16ED();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"01***011", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"LD", target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		if(data.GetBit(3))
		{
			//LD dd,(nn)	11101101 01dd1011 nnnnnnnn nnnnnnnn
			source.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
			target.Decode16BitRegister(data.GetDataSegment(4, 2));
			AddExecuteCycleCount(16);
		}
		else
		{
			//LD (nn),dd	11101101 01dd0011 nnnnnnnn nnnnnnnn
			source.Decode16BitRegister(data.GetDataSegment(4, 2));
			target.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(16);
		}

		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
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
