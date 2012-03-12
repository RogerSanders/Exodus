#include "M68000_Instruction.h"
#ifndef __M68000_MOVE_FROM_CCR_H__
#define __M68000_MOVE_FROM_CCR_H__
namespace M68000 {

class MOVE_from_CCR :public M68000_Instruction
{
public:
	virtual MOVE_from_CCR* Clone() {return new MOVE_from_CCR();}

	virtual bool RegisterOpcode(Processor* processor)
	{
		return processor->RegisterOpcode(this, "0100001011DDDDDD", "DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual int GetInstructionExecuteTime() const
	{
	//##STUB##
		return 1;
	}

	virtual void M68000Decode(M68000* cpu, LONG location, WORD data)
	{
		SetInstructionSize(2);

//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 0 | 0 | 1 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|

		//MOVE	CCR,<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), SIZE_BYTE, location + GetInstructionSize(), cpu);
		AddInstructionSize(target.ExtensionSize());
	}

	virtual void M68000Execute(M68000* cpu, LONG location) const
	{
		location += 2;
		WORD result;

		//##TODO## Add TRAP generation if not in supervisor state
		result = cpu->GetCCR().Convert(SIZE_WORD);
		target.Write(cpu, location, result);

		cpu->SetPC(location + (GetInstructionSize() - 2));
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
