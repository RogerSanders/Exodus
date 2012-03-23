#include "Z80Instruction.h"
#ifndef __Z80_LDD_H__
#define __Z80_LDD_H__
namespace Z80 {

class LDD :public Z80Instruction
{
public:
	virtual LDD* Clone() const {return new LDD();}
	virtual LDD* ClonePlacement(void* buffer) const {return new(buffer) LDD();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"10101000", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LDD";
	}

	virtual Disassembly Z80Disassemble() const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		//LDD		11101101 10101000
		source.SetMode(EffectiveAddress::MODE_HL_POSTDEC);
		target.SetMode(EffectiveAddress::MODE_DE_POSTDEC);
		AddExecuteCycleCount(12);

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
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
		cpu->SetBC(cpu->GetBC() - 1);

		//Set the flag results
		cpu->SetFlagH(false);
		cpu->SetFlagPV(cpu->GetBC() != 0);
		cpu->SetFlagN(false);

		Z80Byte currentRegA;
		cpu->GetA(currentRegA);
		Z80Byte regAResult = currentRegA + result;
		cpu->SetFlagY(regAResult.GetBit(1));
		cpu->SetFlagX(regAResult.GetBit(3));

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
