#include "Z80Instruction.h"
#ifndef __Z80_RLCA_H__
#define __Z80_RLCA_H__
namespace Z80 {

class RLCA :public Z80Instruction
{
public:
	virtual RLCA* Clone() const {return new RLCA();}
	virtual RLCA* ClonePlacement(void* buffer) const {return new(buffer) RLCA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00000111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RLCA";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_target.SetIndexState(GetIndexState(), GetIndexOffset());
		_target.SetMode(EffectiveAddress::Mode::A);
		AddExecuteCycleCount(4);

		AddInstructionSize(GetIndexOffsetSize(_target.UsesIndexOffset()));
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		// Perform the operation
		additionalTime += _target.Read(cpu, location, op1);
		result = (op1 << 1);
		result.SetBit(0, op1.MSB());
		additionalTime += _target.Write(cpu, location, result);

		// Set the flag results
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagN(false);
		cpu->SetFlagC(op1.MSB());

		// Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _target;
};

} // Close namespace Z80
#endif
