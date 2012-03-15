#include "M68000Instruction.h"
#ifndef __M68000_ILLEGAL_H__
#define __M68000_ILLEGAL_H__
namespace M68000 {

class ILLEGAL :public M68000Instruction
{
public:
	virtual ILLEGAL* Clone() {return new ILLEGAL();}
	virtual ILLEGAL	* ClonePlacement(void* buffer) {return new(buffer) ILLEGAL();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"0100101011111100", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"ILLEGAL", L"");
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 0 | 1 | 0 | 1 | 1 | 1 | 1 | 1 | 1 | 0 | 0 |
//	-----------------------------------------------------------------
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		ExecuteTime exceptionTime;

		//Perform the operation
		if(!cpu->ExceptionDisabled(M68000::EX_ILLEGAL_INSTRUCTION))
		{
			cpu->PushStackFrame(cpu->GetPC(), cpu->GetSR());
			exceptionTime = cpu->ProcessException(M68000::EX_ILLEGAL_INSTRUCTION);
		}

		//Return the execution time
		return GetExecuteCycleCount() + exceptionTime;
	}
};

} //Close namespace M68000
#endif
