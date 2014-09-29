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
		if(conditionCode == ConditionCode::None)
		{
			return Disassembly(GetOpcodeName(), source.Disassemble());
		}
		else
		{
			return Disassembly(GetOpcodeName(), DisassembleConditionCode(conditionCode) + L", " + source.Disassemble());
		}
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		conditionCode = ConditionCode::None;

		//CALL nn		//11001101 nnnnnnnn nnnnnnnn
		source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent);
		if(!data.GetBit(0))
		{
			//CALL cc,nn		//11ccc100 nnnnnnnn nnnnnnnn
			conditionCode = (ConditionCode)data.GetDataSegment(3, 3);
		}
		target.SetMode(EffectiveAddress::Mode::SPPreDec);

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		ExecuteTime additionalCycles;

		//Test the condition code
		if(ConditionCodeTrue(cpu, conditionCode))
		{
			//If the condition is true, jump to the target location.
			additionalTime += target.Write(cpu, location + GetInstructionSize(), location + GetInstructionSize());
			Z80Word newPC;
			additionalTime += source.Read(cpu, location + GetInstructionSize(), newPC);
			cpu->PushCallStack(cpu->GetPC().GetData(), newPC.GetData(), (location + GetInstructionSize()).GetData(), L"CALL");
			cpu->SetPC(newPC);
			additionalCycles.cycles = 17;
		}
		else
		{
			//If the condition is false, skip the call, and continue execution at the
			//next instruction.
			cpu->SetPC(location + GetInstructionSize());
			additionalCycles.cycles = 10;
		}

		//Return the execution time
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	ConditionCode conditionCode;
};

} //Close namespace Z80
#endif
