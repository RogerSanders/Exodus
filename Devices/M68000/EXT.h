#include "M68000Instruction.h"
#ifndef __M68000_EXT_H__
#define __M68000_EXT_H__
namespace M68000 {

class EXT :public M68000Instruction
{
public:
	virtual EXT* Clone() {return new EXT();}
	virtual EXT* ClonePlacement(void* buffer) {return new(buffer) EXT();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"010010001*000***", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"EXT." + DisassembleSize(size), target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 0 | 0 | 0 | 1 | S | 0 | 0 | 0 | REGISTER  |
//	-----------------------------------------------------------------
//	                            |--<opmode>-|
		switch(data.GetDataSegment(6, 1))
		{
		case 0: //0
			size = BITCOUNT_BYTE;
			targetSize = BITCOUNT_WORD;
			break;
		case 1:	//1
			size = BITCOUNT_WORD;
			targetSize = BITCOUNT_LONG;
			break;
		}

		//EXT	Dn
		target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size);
		Data result(targetSize);

		//Perform the operation
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op1, GetInstructionRegister());
		result = op1.SignExtend(targetSize);
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

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
	Bitcount targetSize;
};

} //Close namespace M68000
#endif
