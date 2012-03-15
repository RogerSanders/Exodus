#include "M68000Instruction.h"
#ifndef __M68000_TST_H__
#define __M68000_TST_H__
namespace M68000 {

class TST :public M68000Instruction
{
public:
	virtual TST* Clone() {return new TST();}
	virtual TST* ClonePlacement(void* buffer) {return new(buffer) TST();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"01001010CCDDDDDD", L"CC=00 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011");
		result &= table.AllocateRegionToOpcode(this, L"01001010CCDDDDDD", L"CC=01-10 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
		return result;
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"TST." + DisassembleSize(size), target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|-------|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 0 | 1 | 0 | SIZE  |   MODE    |  REGISTER |
//	----------------------------------------=========================
//                                          |----------<ea>---------|
		switch(data.GetDataSegment(6, 2))
		{
		case 0:	//00
			size = BITCOUNT_BYTE;
			break;
		case 1:	//01
			size = BITCOUNT_WORD;
			break;
		case 2:	//10
			size = BITCOUNT_LONG;
			break;
		}

		//TST	<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data result(size);

		//Perform the operation
		additionalTime += target.Read(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		cpu->SetC(false);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
