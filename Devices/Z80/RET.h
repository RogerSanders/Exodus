#include "Z80Instruction.h"
#ifndef __Z80_RET_H__
#define __Z80_RET_H__
namespace Z80 {

class RET :public Z80Instruction
{
public:
	virtual RET* Clone() const {return new RET();}
	virtual RET* ClonePlacement(void* buffer) const {return new(buffer) RET();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"11001001", L"");
		result &= table.AllocateRegionToOpcode(this, L"11***000", L"");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RET";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		if(conditionCode == ConditionCode::None)
		{
			return Disassembly(GetOpcodeName(), L"");
		}
		else
		{
			return Disassembly(GetOpcodeName(), DisassembleConditionCode(conditionCode));
		}
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		conditionCode = ConditionCode::None;

		if(data.GetBit(0))
		{
			//RET		//11001001
		}
		else
		{
			//RET cc		//11ccc000
			conditionCode = (ConditionCode)data.GetDataSegment(3, 3);
			AddExecuteCycleCount(1);
		}
		target.SetMode(EffectiveAddress::Mode::SPPostInc);

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		ExecuteTime additionalCycles;

		//Test the condition code
		if(ConditionCodeTrue(cpu, conditionCode))
		{
			//If the condition is true, restore the PC from the stack.
			Z80Word oldPC;
			additionalTime += target.Read(cpu, location + GetInstructionSize(), oldPC);
			cpu->PopCallStack(oldPC.GetData());
			cpu->SetPC(oldPC);
			additionalCycles.cycles = 10;
		}
		else
		{
			//If the condition is false, continue execution at the next instruction.
			cpu->SetPC(location + GetInstructionSize());
			additionalCycles.cycles = 4;
		}

		//Return the execution time
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	EffectiveAddress target;
	ConditionCode conditionCode;
};

} //Close namespace Z80
#endif
