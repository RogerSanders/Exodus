#include "Z80Instruction.h"
#ifndef __Z80_XOR8_H__
#define __Z80_XOR8_H__
namespace Z80 {

class XOR8 :public Z80Instruction
{
public:
	virtual XOR8* Clone() {return new XOR8();}
	virtual XOR8* ClonePlacement(void* buffer) {return new(buffer) XOR8();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"10101***", L"");
		result &= table.AllocateRegionToOpcode(this, L"11101110", L"");
		return result;
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"XOR", target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_A);

		if(source.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//XOR A,r		10101rrr
			AddExecuteCycleCount(4);
		}
		else if(data.GetBit(6))
		{
			//XOR A,n		11101110
			source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(7);
		}
		else
		{
			//XOR A,(HL)		10101110
			//XOR A,(IX + d)	11011101 10101110 dddddddd
			//XOR A,(IY + d)	11111101 10101110 dddddddd
			source.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
			if(GetIndexState() == EffectiveAddress::INDEX_NONE)
			{
				AddExecuteCycleCount(7);
			}
			else
			{
				AddExecuteCycleCount(15);
			}
		}

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte op2;
		Z80Byte result;

		//Perform the operation
		additionalTime += source.Read(cpu, location, op1);
		additionalTime += target.Read(cpu, location, op2);
		result = op2 ^ op1;
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV(result.ParityEven());
		cpu->SetFlagN(false);
		cpu->SetFlagC(false);

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
