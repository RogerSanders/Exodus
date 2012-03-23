#include "Z80Instruction.h"
#ifndef __Z80_RRCA_H__
#define __Z80_RRCA_H__
namespace Z80 {

class RRCA :public Z80Instruction
{
public:
	virtual RRCA* Clone() const {return new RRCA();}
	virtual RRCA* ClonePlacement(void* buffer) const {return new(buffer) RRCA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00001111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RRCA";
	}

	virtual Disassembly Z80Disassemble() const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_A);
		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		//Perform the operation
		additionalTime += target.Read(cpu, location, op1);
		result = (op1 >> 1);
		result.SetBit(result.GetBitCount() - 1, op1.GetBit(0));
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagN(false);
		cpu->SetFlagC(op1.GetBit(0));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
