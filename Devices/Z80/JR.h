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

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		if(_conditionCode == ConditionCode::None)
		{
			return Disassembly(GetOpcodeName(), _source.Disassemble(), _source.DisassembleImmediateAsPCDisplacement(GetInstructionLocation() + GetInstructionSize()));
		}
		else
		{
			return Disassembly(GetOpcodeName(), DisassembleConditionCode(_conditionCode) + L", " + _source.Disassemble(), _source.DisassembleImmediateAsPCDisplacement(GetInstructionLocation() + GetInstructionSize()));
		}
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_source.SetIndexState(GetIndexState(), GetIndexOffset());

		//JR e			00011000 eeeeeeee
		_conditionCode = ConditionCode::None;
		if(data.GetDataSegment(3, 3) != 3)
		{
			//JR NZ,e		00100000 eeeeeeee
			//JR Z,e		00101000 eeeeeeee
			//JR NC,e		00110000 eeeeeeee
			//JR C,e		00111000 eeeeeeee
			_conditionCode = (ConditionCode)data.GetDataSegment(3, 2);
		}
		_source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
		AddExecuteCycleCount(7);

		AddInstructionSize(GetIndexOffsetSize(_source.UsesIndexOffset()));
		AddInstructionSize(_source.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte offset;
		Z80Word newPC;
		ExecuteTime additionalCycles;

		//Test the condition code
		if(ConditionCodeTrue(cpu, _conditionCode))
		{
			//If the condition is true, jump to the _target location.
			additionalTime += _source.Read(cpu, location, offset);
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
	EffectiveAddress _source;
	ConditionCode _conditionCode;
};

} //Close namespace Z80
#endif
