#include "Z80Instruction.h"
#ifndef __Z80_JR_H__
#define __Z80_JR_H__
namespace Z80 {

class JR :public Z80Instruction
{
public:
	virtual JR* Clone() const {return new JR();}
	virtual JR* ClonePlacement(void* buffer) const {return new(buffer) JR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00AAA000", L"AAA=011-111");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"JR";
	}

	virtual Disassembly Z80Disassemble() const
	{
		if(conditionCode == CONDITIONCODE_NONE)
		{
			return Disassembly(GetOpcodeName(), source.Disassemble(), source.DisassembleImmediateAsPCDisplacement(GetInstructionLocation() + GetInstructionSize()));
		}
		else
		{
			return Disassembly(GetOpcodeName(), DisassembleConditionCode(conditionCode) + L", " + source.Disassemble(), source.DisassembleImmediateAsPCDisplacement(GetInstructionLocation() + GetInstructionSize()));
		}
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());

		//JR e			00011000 eeeeeeee
		conditionCode = CONDITIONCODE_NONE;
		if(data.GetDataSegment(3, 3) != 3)
		{
			//JR NZ,e		00100000 eeeeeeee
			//JR Z,e		00101000 eeeeeeee
			//JR NC,e		00110000 eeeeeeee
			//JR C,e		00111000 eeeeeeee
			conditionCode = (ConditionCode)data.GetDataSegment(3, 2);
		}
		source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
		AddExecuteCycleCount(7);

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte offset;
		Z80Word newPC;
		ExecuteTime additionalCycles;

		//Test the condition code
		if(ConditionCodeTrue(cpu, conditionCode))
		{
			//If the condition is true, jump to the target location.
			additionalTime += source.Read(cpu, location, offset);
			newPC = location + GetInstructionSize();
			newPC += Z80Word(offset.SignExtend(BITCOUNT_WORD));
			additionalCycles.cycles = 5;
		}
		else
		{
			//If the condition is false, skip the jump, and continue execution at the
			//next instruction.
			newPC = location + GetInstructionSize();
		}
		cpu->SetPC(newPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	EffectiveAddress source;
	ConditionCode conditionCode;
};

} //Close namespace Z80
#endif
