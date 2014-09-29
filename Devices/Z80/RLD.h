#include "Z80Instruction.h"
#ifndef __Z80_RLD_H__
#define __Z80_RLD_H__
namespace Z80 {

class RLD :public Z80Instruction
{
public:
	virtual RLD* Clone() const {return new RLD();}
	virtual RLD* ClonePlacement(void* buffer) const {return new(buffer) RLD();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01101111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RLD";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		hl.SetIndexState(GetIndexState(), GetIndexOffset());
		a.SetIndexState(GetIndexState(), GetIndexOffset());

		//RLD		11101101 01101111
		hl.SetMode(EffectiveAddress::Mode::HLIndirect);
		a.SetMode(EffectiveAddress::Mode::A);
		AddExecuteCycleCount(14);

		AddInstructionSize(GetIndexOffsetSize(hl.UsesIndexOffset() || a.UsesIndexOffset()));
		AddInstructionSize(hl.ExtensionSize());
		AddInstructionSize(a.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte hlSource;
		Z80Byte aSource;
		Z80Byte hlResult;
		Z80Byte aResult;

		//Perform the operation
		additionalTime += hl.Read(cpu, location, hlSource);
		additionalTime += a.Read(cpu, location, aSource);

		aResult = aSource;
		aResult.SetDataSegment(0, 4, hlSource.GetDataSegment(4, 4));
		hlResult.SetDataSegment(4, 4, hlSource.GetDataSegment(0, 4));
		hlResult.SetDataSegment(0, 4, aSource.GetDataSegment(0, 4));

		additionalTime += hl.Write(cpu, location, hlResult);
		additionalTime += a.Write(cpu, location, aResult);

		//Set the flag results
		cpu->SetFlagS(aResult.Negative());
		cpu->SetFlagZ(aResult.Zero());
		cpu->SetFlagY(aResult.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(aResult.GetBit(3));
		cpu->SetFlagPV(aResult.ParityEven());
		cpu->SetFlagN(false);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress hl;
	EffectiveAddress a;
};

} //Close namespace Z80
#endif
