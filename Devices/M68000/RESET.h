#include "M68000Instruction.h"
#ifndef __M68000_RESET_H__
#define __M68000_RESET_H__
namespace M68000 {

class RESET :public M68000Instruction
{
public:
	virtual RESET* Clone() {return new RESET();}
	virtual RESET* ClonePlacement(void* buffer) {return new(buffer) RESET();}

	virtual bool Privileged() const
	{
		return true;
	}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"0100111001110000", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"RESET", L"");
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 0 | 0 |
//	-----------------------------------------------------------------
		//The RESET opcode takes 4 cycles to execute, plus another 128 cycles for the
		//length of time during which the RESET line is asserted, giving a total duration
		//of 132 cycles. We add the 128 reset cycles to the total execution time during
		//the execution stage.
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		//Calculate the exact time periods when the reset line is asserted and negated
		unsigned int resetLineAssertCycles = 128;
		double resetBeginTime = cpu->CalculateExecutionTime(GetExecuteCycleCount().cycles) + cpu->GetCurrentTimesliceProgress();
		double resetEndTime = cpu->CalculateExecutionTime(GetExecuteCycleCount().cycles + resetLineAssertCycles) + cpu->GetCurrentTimesliceProgress();
		ExecuteTime additionalCycles;
		additionalCycles.cycles = resetLineAssertCycles;

		//Toggle the external RESET line state, to reset external devices.
		cpu->TriggerExternalReset(resetBeginTime, resetEndTime);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount() + additionalCycles;
	}
};

} //Close namespace M68000
#endif
