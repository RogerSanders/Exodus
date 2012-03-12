#include "M68000Instruction.h"
#ifndef __M68000_MOVEQ_H__
#define __M68000_MOVEQ_H__
namespace M68000 {

class MOVEQ :public M68000Instruction
{
public:
	virtual MOVEQ* Clone() {return new MOVEQ();}
	virtual MOVEQ* ClonePlacement(void* buffer) {return new(buffer) MOVEQ();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"0111***0********", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"MOVEQ", source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------------------------------|
//	| 0 | 1 | 1 | 1 | REGISTER  | 0 |        IMMEDIATE DATA         |
//	-----------------------------------------------------------------

		source.BuildImmediateData(location + GetInstructionSize(), M68000Byte(data.GetDataSegment(0, 8)));
		target.BuildDataDirect(BITCOUNT_BYTE, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Byte result;

		//Perform the operation
		additionalTime += source.Read(cpu, result, GetInstructionRegister());
		additionalTime += target.Write(cpu, result.SignExtend(BITCOUNT_LONG), GetInstructionRegister());

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
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
