#include "Z80Instruction.h"
#ifndef __Z80_RET_H__
#define __Z80_RET_H__
namespace Z80 {

class RET :public Z80Instruction
{
public:
	virtual RET* Clone() {return new RET();}
	virtual RET* ClonePlacement(void* buffer) {return new(buffer) RET();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		bool result = true;
		result &= table->AllocateRegionToOpcode(this, L"11001001", L"");
		result &= table->AllocateRegionToOpcode(this, L"11***000", L"");
		return result;
	}

	virtual Disassembly Z80Disassemble()
	{
		if(conditionCode == CONDITIONCODE_NONE)
		{
			return Disassembly(L"RET", L"");
		}
		else
		{
			return Disassembly(L"RET", DisassembleConditionCode(conditionCode));
		}
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		conditionCode = CONDITIONCODE_NONE;

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
		target.SetMode(EffectiveAddress::MODE_SP_POSTINC);

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
