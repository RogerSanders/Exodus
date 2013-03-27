#include "Z80Instruction.h"
#ifndef __Z80_RES_H__
#define __Z80_RES_H__
namespace Z80 {

class RES :public Z80Instruction
{
public:
	virtual RES* Clone() const {return new RES();}
	virtual RES* ClonePlacement(void* buffer) const {return new(buffer) RES();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"10******", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RES";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		doubleOutput = false;

		if(target.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//RES b,r		11001011 10bbbrrr
			AddExecuteCycleCount(4);
		}
		else
		{
			//RES b,(HL)		11001011 10bbb110
			//RES b,(IX+d)		11011101 11001011 dddddddd 10bbb110
			//RES b,(IY+d)		11111101 11001011 dddddddd 10bbb110
			target.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
			AddExecuteCycleCount(7);

			if(GetIndexState() != EffectiveAddress::INDEX_NONE)
			{
				doubleOutput = true;
				targetHL.SetIndexState(GetIndexState(), GetIndexOffset());
				targetHL.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
				AddExecuteCycleCount(8);
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
		op1.SetBit(bitNumber.GetData(), false);
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
