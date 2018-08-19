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

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		if (_conditionCode == ConditionCode::None)
		{
			return Disassembly(GetOpcodeName(), _source.Disassemble());
		}
		else
		{
			return Disassembly(GetOpcodeName(), DisassembleConditionCode(_conditionCode) + L", " + _source.Disassemble());
		}
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_source.SetIndexState(GetIndexState(), GetIndexOffset());

		_conditionCode = ConditionCode::None;
		if (data.GetDataSegment(0, 3) == 0x01)
		{
			//##NOTE## This is NOT indirect, despite the notation.
			//JP (HL)		11101001
			//JP (IX)		11011101 11101001
			//JP (IY)		11111101 11101001
			_source.SetMode(EffectiveAddress::Mode::HL);
			AddExecuteCycleCount(4);
		}
		else if (data.GetDataSegment(0, 3) == 0x03)
		{
			//JP nn			11000011 nnnnnnnn nnnnnnnn
			_source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(10);
		}
		else
		{
			//JP cc,nn		11***010 nnnnnnnn nnnnnnnn
			_source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent);
			_conditionCode = (ConditionCode)data.GetDataSegment(3, 3);
			AddExecuteCycleCount(10);
		}

		AddInstructionSize(GetIndexOffsetSize(_source.UsesIndexOffset()));
		AddInstructionSize(_source.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word newPC;

		//Test the condition code
		if (ConditionCodeTrue(cpu, _conditionCode))
		{
			//If the condition is true, jump to the _target location.
			additionalTime += _source.Read(cpu, location, newPC);
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
	EffectiveAddress _source;
	ConditionCode _conditionCode;
};

} //Close namespace Z80
#endif
