#include "M68000Instruction.h"
#ifndef __M68000_TRAP_H__
#define __M68000_TRAP_H__
namespace M68000 {

class TRAP :public M68000Instruction
{
public:
	virtual TRAP* Clone() {return new TRAP();}
	virtual TRAP* ClonePlacement(void* buffer) {return new(buffer) TRAP();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"010011100100****", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		//##TODO## Clean this up
		EffectiveAddress source;
		source.BuildImmediateData(0, M68000Byte(trapNo));
		return Disassembly(L"TRAP", source.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---------------|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 0 | 0 |  N° of TRAP   |
//	-----------------------------------------------------------------

		trapNo = data.GetDataSegment(0, 4);
		AddExecuteCycleCount(ExecuteTime(4, 0, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		ExecuteTime exceptionTime;

		//Perform the operation
		if(!cpu->ExceptionDisabled(M68000::EX_INTERRUPT_TRAP + trapNo))
		{
			cpu->SetPC(location + GetInstructionSize());
			cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
			exceptionTime = cpu->ProcessException(M68000::EX_INTERRUPT_TRAP + trapNo);
		}

		//Return the execution time
		return GetExecuteCycleCount() + exceptionTime;
	}

private:
	unsigned int trapNo;
};

} //Close namespace M68000
#endif
