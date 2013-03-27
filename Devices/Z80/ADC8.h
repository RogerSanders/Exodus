#include "Z80Instruction.h"
#ifndef __Z80_ADC8_H__
#define __Z80_ADC8_H__
namespace Z80 {

class ADC8 :public Z80Instruction
{
public:
	virtual ADC8* Clone() const {return new ADC8();}
	virtual ADC8* ClonePlacement(void* buffer) const {return new(buffer) ADC8();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"10001***", L"");
		result &= table.AllocateRegionToOpcode(this, L"11001110", L"");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ADC";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_A);

		if(source.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//ADC A,r		10001rrr
			AddExecuteCycleCount(4);
		}
		else if(data.GetBit(6))
		{
			//ADC A,n		11001110 nnnnnnnn
			source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(7);
		}
		else
		{
			//ADC A,(HL)	10001110
			//ADC A,(IX+d)	11011110 10001110 dddddddd
			//ADC A,(IY+d)	11111110 10001110 dddddddd
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
		Z80Byte carry;

		//Perform the operation
		additionalTime += source.Read(cpu, location, op1);
		additionalTime += target.Read(cpu, location, op2);
		carry = (unsigned int)cpu->GetFlagC();
		result = op2 + op1 + carry;
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH((op1.GetDataSegment(0, 4) + op2.GetDataSegment(0, 4) + carry.GetData()) > result.GetDataSegment(0, 4));
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV((op1.MSB() == op2.MSB()) && (result.MSB() != op1.MSB()));
		cpu->SetFlagN(false);
		cpu->SetFlagC((op1.GetData() + op2.GetData() + carry.GetData()) > result.GetData());

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
