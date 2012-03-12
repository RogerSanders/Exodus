#include "M68000_Instruction.h"
#ifndef __M68000_BKPT_H__
#define __M68000_BKPT_H__
namespace M68000 {

class BKPT :public M68000_Instruction
{
public:
	virtual BKPT* Clone() {return new BKPT();}

	virtual bool RegisterOpcode(OpcodeTable* table)
	{
		return table->AllocateRegionToOpcode(this, "0100100001001***", "");
	}

	//virtual int GetInstructionExecuteTime() const
	//{
	////##STUB##
	//	return 1;
	//}

	virtual void M68000Decode(M68000* cpu, LONG location, WORD data)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 1 |N° BREAKPT.|
//	-----------------------------------------------------------------
		breakpoint = data.GetDataSegment(0, 3);
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, LONG location) const
	{
		location += 2;

		//##TODO## Check if we should be incrementing the PC or if the PC should point to the address of this instruction
		//##NOTE## Technically, on an M68000 this is an unimplemented opcode. We should probably drop this opcode completely.
		cpu->SetPC(location + (GetInstructionSize() - 2));
		cpu->PushStackFrame(M68000::EX_ILLEGAL_INSTRUCTION);
		cpu->ProcessException(M68000::EX_ILLEGAL_INSTRUCTION);
		return GetExecuteCycleCount();
	}

private:
	unsigned int breakpoint;
};

} //Close namespace M68000
#endif
