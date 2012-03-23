#include "Z80Instruction.h"
#ifndef __Z80_SRA_H__
#define __Z80_SRA_H__
namespace Z80 {

class SRA :public Z80Instruction
{
public:
	virtual SRA* Clone() const {return new SRA();}
	virtual SRA* ClonePlacement(void* buffer) const {return new(buffer) SRA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00101***", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"SRA";
	}

	virtual Disassembly Z80Disassemble() const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		doubleOutput = false;

		if(target.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//SRA r			11001011 00101rrr
			AddExecuteCycleCount(4);
		}
		else
		{
			//SRA (HL)		11001011 00101110
			//SRA (IX+d)	11011101 11001011 dddddddd 00101110
			//SRA (IY+d)	11111101 11001011 dddddddd 00101110
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

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		//Perform the operation
		if(doubleOutput)
		{
			additionalTime += targetHL.Read(cpu, location, op1);
		}
		else
		{
			additionalTime += target.Read(cpu, location, op1);
		}
		result = (op1 >> 1);
		result.SetBit(result.GetBitCount() - 1, op1.MSB());
		if(doubleOutput)
		{
			additionalTime += targetHL.Write(cpu, location, result);
		}
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV(result.ParityEven());
		cpu->SetFlagN(false);
		cpu->SetFlagC(op1.GetBit(0));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
	EffectiveAddress targetHL;
	bool doubleOutput;
};

} //Close namespace Z80
#endif
