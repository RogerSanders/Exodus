#include "Z80Instruction.h"
#ifndef __Z80_LD8ED_H__
#define __Z80_LD8ED_H__
namespace Z80 {

class LD8ED :public Z80Instruction
{
public:
	virtual LD8ED* Clone() {return new LD8ED();}
	virtual LD8ED* ClonePlacement(void* buffer) {return new(buffer) LD8ED();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"010**111", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"LD", target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		switch(data.GetDataSegment(3, 2))
		{
		case 0:
			//LD I,A		11101101 01000111
			source.SetMode(EffectiveAddress::MODE_A);
			target.SetMode(EffectiveAddress::MODE_I);
			break;
		case 1:
			//LD R,A		11101101 01001111
			source.SetMode(EffectiveAddress::MODE_A);
			target.SetMode(EffectiveAddress::MODE_R);
			break;
		case 2:
			//LD A,I		11101101 01010111
			source.SetMode(EffectiveAddress::MODE_I);
			target.SetMode(EffectiveAddress::MODE_A);
			break;
		case 3:
			//LD A,R		11101101 01011111
			source.SetMode(EffectiveAddress::MODE_R);
			target.SetMode(EffectiveAddress::MODE_A);
			break;
		}
		AddExecuteCycleCount(5);

		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte result;

		//Perform the operation
		additionalTime += source.Read(cpu, location, result);
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		if(target.GetMode() == EffectiveAddress::MODE_A)
		{
			cpu->SetFlagS(result.MSB());
			cpu->SetFlagZ(result.Zero());
			cpu->SetFlagY(result.GetBit(5));
			cpu->SetFlagH(false);
			cpu->SetFlagX(result.GetBit(3));
			cpu->SetFlagPV(cpu->GetIFF2());
			cpu->SetFlagN(false);
		}

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
