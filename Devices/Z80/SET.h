#include "Z80Instruction.h"
#ifndef __Z80_SET_H__
#define __Z80_SET_H__
namespace Z80 {

class SET :public Z80Instruction
{
public:
	virtual SET* Clone() {return new SET();}
	virtual SET* ClonePlacement(void* buffer) {return new(buffer) SET();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"11******", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"SET", target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		doubleOutput = false;

		if(target.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//SET b,r		11001011 11bbbrrr
			AddExecuteCycleCount(4);
		}
		else
		{
			//SET b,(HL)		11001011 11bbb110
			//SET b,(IX+d)		11011101 11001011 dddddddd 11bbb110
			//SET b,(IY+d)		11111101 11001011 dddddddd 11bbb110
			target.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
			AddExecuteCycleCount(11);

			if(GetIndexState() != EffectiveAddress::INDEX_NONE)
			{
				doubleOutput = true;
				targetHL.SetIndexState(GetIndexState(), GetIndexOffset());
				targetHL.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
				AddExecuteCycleCount(4);
			}
		}
		source.BuildQuickData(data.GetDataSegment(3, 3));

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;

		//Perform the operation
		if(doubleOutput)
		{
			additionalTime += targetHL.Read(cpu, location, op1);
		}
		else
		{
			additionalTime += target.Read(cpu, location, op1);
		}
		Z80Byte bitNumber;
		additionalTime += source.Read(cpu, location, bitNumber);
		op1.SetBit(bitNumber.GetData(), true);
		if(doubleOutput)
		{
			additionalTime += targetHL.Write(cpu, location, op1);
		}
		additionalTime += target.Write(cpu, location, op1);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	EffectiveAddress targetHL;
	bool doubleOutput;
};

} //Close namespace Z80
#endif
