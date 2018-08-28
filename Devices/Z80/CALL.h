#include "Z80Instruction.h"
#ifndef __Z80_CALL_H__
#define __Z80_CALL_H__
namespace Z80 {

class CALL :public Z80Instruction
{
public:
	virtual CALL* Clone() const {return new CALL();}
	virtual CALL* ClonePlacement(void* buffer) const {return new(buffer) CALL();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"11001101", L"");
		result &= table.AllocateRegionToOpcode(this, L"11***100", L"");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"CALL";
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

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_source.SetIndexState(GetIndexState(), GetIndexOffset());
		_target.SetIndexState(GetIndexState(), GetIndexOffset());
		_conditionCode = ConditionCode::None;

		// CALL nn		// 11001101 nnnnnnnn nnnnnnnn
		_source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent);
		if (!data.GetBit(0))
		{
			// CALL cc,nn		// 11ccc100 nnnnnnnn nnnnnnnn
			_conditionCode = (ConditionCode)data.GetDataSegment(3, 3);
		}
		_target.SetMode(EffectiveAddress::Mode::SPPreDec);

		AddInstructionSize(GetIndexOffsetSize(_source.UsesIndexOffset() || _target.UsesIndexOffset()));
		AddInstructionSize(_source.ExtensionSize());
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		ExecuteTime additionalCycles;

		// Test the condition code
		if (ConditionCodeTrue(cpu, _conditionCode))
		{
			// If the condition is true, jump to the _target location.
			additionalTime += _target.Write(cpu, location + GetInstructionSize(), location + GetInstructionSize());
			Z80Word newPC;
			additionalTime += _source.Read(cpu, location + GetInstructionSize(), newPC);
			cpu->PushCallStack(cpu->GetPC().GetData(), newPC.GetData(), (location + GetInstructionSize()).GetData(), L"CALL");
			cpu->SetPC(newPC);
			additionalCycles.cycles = 17;
		}
		else
		{
			// If the condition is false, skip the call, and continue execution at the
			// next instruction.
			cpu->SetPC(location + GetInstructionSize());
			additionalCycles.cycles = 10;
		}

		// Return the execution time
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
	ConditionCode _conditionCode;
};

} // Close namespace Z80
#endif
