#include "Z80Instruction.h"
#ifndef __Z80_JP_H__
#define __Z80_JP_H__
namespace Z80 {

class JP :public Z80Instruction
{
public:
	virtual JP* Clone() const {return new JP();}
	virtual JP* ClonePlacement(void* buffer) const {return new(buffer) JP();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"11101001", L"");
		result &= table.AllocateRegionToOpcode(this, L"11000011", L"");
		result &= table.AllocateRegionToOpcode(this, L"11***010", L"");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"JP";
	}

	virtual Disassembly Z80Disassemble() const
	{
		if(conditionCode == CONDITIONCODE_NONE)
		{
			return Disassembly(GetOpcodeName(), source.Disassemble());
		}
		else
		{
			return Disassembly(GetOpcodeName(), DisassembleConditionCode(conditionCode) + L", " + source.Disassemble());
		}
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());

		conditionCode = CONDITIONCODE_NONE;
		if(data.GetDataSegment(0, 3) == 0x01)
		{
			//##NOTE## This is NOT indirect, despite the notation.
			//JP (HL)		11101001
			//JP (IX)		11011101 11101001
			//JP (IY)		11111101 11101001
			source.SetMode(EffectiveAddress::MODE_HL);
			AddExecuteCycleCount(4);
		}
		else if(data.GetDataSegment(0, 3) == 0x03)
		{
			//JP nn			11000011 nnnnnnnn nnnnnnnn
			source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(10);
		}
		else
		{
			//JP cc,nn		11***010 nnnnnnnn nnnnnnnn
			source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent);
			conditionCode = (ConditionCode)data.GetDataSegment(3, 3);
			AddExecuteCycleCount(10);
		}

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word newPC;

		//Test the condition code
		if(ConditionCodeTrue(cpu, conditionCode))
		{
			//If the condition is true, jump to the target location.
			additionalTime += source.Read(cpu, location, newPC);
		}
		else
		{
			//If the condition is false, skip the jump, and continue execution at the
			//next instruction.
			newPC = (location + GetInstructionSize());
		}
		cpu->SetPC(newPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	ConditionCode conditionCode;
};

} //Close namespace Z80
#endif
