#include "M68000Instruction.h"
#ifndef __M68000_TRAPV_H__
#define __M68000_TRAPV_H__
namespace M68000 {

class TRAPV :public M68000Instruction
{
public:
	virtual TRAPV* Clone() {return new TRAPV();}
	virtual TRAPV* ClonePlacement(void* buffer) {return new(buffer) TRAPV();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"0100111001110110", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"TRAPV", L"");
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 | 1 | 0 |
//	-----------------------------------------------------------------
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		ExecuteTime additionalCycles;

		//Perform the operation
		cpu->SetPC(location + GetInstructionSize());
		if(cpu->GetV())
		{
			if(!cpu->ExceptionDisabled(M68000::EX_TRAPV_INSTRUCTION))
			{
				cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
				additionalCycles = cpu->ProcessException(M68000::EX_TRAPV_INSTRUCTION);
			}
		}

		//Return the execution time
		return GetExecuteCycleCount() + additionalCycles;
	}
};

} //Close namespace M68000
#endif
