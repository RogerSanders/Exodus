#include "M68000Instruction.h"
#ifndef __M68000_ADDX_H__
#define __M68000_ADDX_H__
namespace M68000 {

class ADDX :public M68000Instruction
{
public:
	virtual ADDX* Clone() {return new ADDX();}
	virtual ADDX* ClonePlacement(void* buffer) {return new(buffer) ADDX();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"1101***1CC00****", L"CC=00-10");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"ADDX." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|---|---|---|-----------|
//	| 1 | 1 | 0 | 1 |    Rx     | 1 | SIZE  | 0 | 0 |R/M|    Ry     |
//	-----------------------------------------------------------------
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

		if(!data.GetBit(3))
		{
			//ADDX	Dy,Dx
			source.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 0));
			}
		}
		else
		{
			//ADDX	-(Ay),-(Ax)
			source.BuildAddressPredec(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			target.BuildAddressPredec(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(18, 3, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(30, 5, 2));
			}
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size);
		Data op2(size);
		Data result(size);

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		result = op2 + op1 + cpu->GetX();
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		bool carry = (op1.MSB() && op2.MSB()) || (!result.MSB() && (op1.MSB() || op2.MSB())) ;
		bool overflow = (op1.MSB() == op2.MSB()) && (result.MSB() != (op1.MSB() && op2.MSB()));
		cpu->SetX(carry);
		cpu->SetN(result.Negative());
		cpu->SetZ(cpu->GetZ() && result.Zero());
		cpu->SetV(overflow);
		cpu->SetC(carry);

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
