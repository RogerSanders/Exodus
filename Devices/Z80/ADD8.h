#include "Z80Instruction.h"
#ifndef __Z80_ADD8_H__
#define __Z80_ADD8_H__
namespace Z80 {

class ADD8 :public Z80Instruction
{
public:
	virtual ADD8* Clone() const {return new ADD8();}
	virtual ADD8* ClonePlacement(void* buffer) const {return new(buffer) ADD8();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"10000***", L"");
		result &= table.AllocateRegionToOpcode(this, L"11000110", L"");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ADD";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::Mode::A);

		if(source.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//ADD A,r		10000rrr
			AddExecuteCycleCount(4);
		}
		else if(data.GetBit(6))
		{
			//ADD A,n		11000110
			source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(7);
		}
		else
		{
			//ADD A,(HL)		10000110
			//ADD A,(IX + d)	11011101 10000110 dddddddd
			//ADD A,(IY + d)	11111101 10000110 dddddddd
			source.SetMode(EffectiveAddress::Mode::HLIndirect);
			if(GetIndexState() == EffectiveAddress::IndexState::None)
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
		result = op2 + op1;
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH((op1.GetDataSegment(0, 4) + op2.GetDataSegment(0, 4)) > result.GetDataSegment(0, 4));
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV((op1.MSB() == op2.MSB()) && (result.MSB() != op1.MSB()));
		cpu->SetFlagN(false);
		cpu->SetFlagC((op1.GetData() + op2.GetData()) > result.GetData());

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
