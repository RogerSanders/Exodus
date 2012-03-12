#include "M68000Instruction.h"
#ifndef __M68000_SUBA_H__
#define __M68000_SUBA_H__
namespace M68000 {

class SUBA :public M68000Instruction
{
public:
	virtual SUBA* Clone() {return new SUBA();}
	virtual SUBA* ClonePlacement(void* buffer) {return new(buffer) SUBA();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"1001***CC1DDDDDD", L"CC=01,11 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"SUBA." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|-------|---|-----------|-----------|
//	| 1 | 0 | 0 | 1 |  REGISTER |  SIZE | 1 |    MODE   |  REGISTER |
//	----------------------------------------=========================
//                              |--<opmode>-|----------<ea>---------|
		switch(data.GetDataSegment(7, 2))
		{
		case 1:	//01
			size = BITCOUNT_WORD;
			break;
		case 3:	//11
			size = BITCOUNT_LONG;
			break;
		}

		//SUBA	<ea>,An
		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		target.BuildAddressDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));

		if(size == BITCOUNT_LONG)
		{
			AddExecuteCycleCount(ExecuteTime(6, 1, 0));
			if((source.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT) || (source.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT) || (source.GetAddressMode() == EffectiveAddress::IMMEDIATE))
			{
				AddExecuteCycleCount(ExecuteTime(2, 0, 0));
			}
		}
		else
		{
			//##TODO## These times look a bit funny. Find a second source to verify execution times against.
			AddExecuteCycleCount(ExecuteTime(8, 1, 0));
		}
		AddExecuteCycleCount(source.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long op1;
		M68000Long op2;
		M68000Long result;
		Data temp(size);

		//Perform the operation
		additionalTime += source.Read(cpu, temp, GetInstructionRegister());
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		op1 = M68000Long(temp.SignExtend(BITCOUNT_LONG));
		result = op2 - op1;
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
