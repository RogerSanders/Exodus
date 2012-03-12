#include "Z80Instruction.h"
#ifndef __Z80_ADC16_H__
#define __Z80_ADC16_H__
namespace Z80 {

class ADC16 :public Z80Instruction
{
public:
	virtual ADC16* Clone() {return new ADC16();}
	virtual ADC16* ClonePlacement(void* buffer) {return new(buffer) ADC16();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"01**1010", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"ADC", target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_HL);

		//ADC HL,ss		11101101 01ss1010
		//ADC IX,ss		11011101 11101101 01ss1010
		//ADC IY,ss		11111101 11101101 01ss1010
		source.Decode16BitRegister(data.GetDataSegment(4, 2));

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(11);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word op1;
		Z80Word op2;
		Z80Word result;
		Z80Word carry;

		//Perform the operation
		additionalTime += source.Read(cpu, location, op1);
		additionalTime += target.Read(cpu, location, op2);
		carry = (unsigned int)cpu->GetFlagC();
		result = op2 + op1 + carry;
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(8+5));
		cpu->SetFlagH((op1.GetDataSegment(0, 8+4) + op2.GetDataSegment(0, 8+4) + carry.GetData()) > result.GetDataSegment(0, 8+4));
		cpu->SetFlagX(result.GetBit(8+3));
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
