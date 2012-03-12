#include "M68000Instruction.h"
#ifndef __M68000_STOP_H__
#define __M68000_STOP_H__
namespace M68000 {

class STOP :public M68000Instruction
{
public:
	virtual STOP* Clone() {return new STOP();}
	virtual STOP* ClonePlacement(void* buffer) {return new(buffer) STOP();}

	virtual bool Privileged() const
	{
		return true;
	}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"0100111001110010", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"STOP", source.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 0 |
//	|---------------------------------------------------------------|
//	|                    16 BITS IMMEDIATE DATA                     |
//	-----------------------------------------------------------------
		source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(4, 0, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Word newSR;

		//Perform the operation
		additionalTime += source.Read(cpu, newSR, GetInstructionRegister());
		cpu->SetSR(newSR);
		//##TODO## We had some code here to generate a privilege violation if the new
		//value for SR didn't have the supervisor bit set. Do some more research, and see
		//if we can find some supporting evidence for this behaviour.
		cpu->SetProcessorState(M68000::STATE_STOPPED);

		//Return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
};

} //Close namespace M68000
#endif
