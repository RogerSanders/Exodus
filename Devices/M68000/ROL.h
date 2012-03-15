#include "M68000Instruction.h"
#ifndef __M68000_ROL_H__
#define __M68000_ROL_H__
namespace M68000 {

class ROL :public M68000Instruction
{
public:
	virtual ROL* Clone() {return new ROL();}
	virtual ROL* ClonePlacement(void* buffer) {return new(buffer) ROL();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1110***1CC*11***", L"CC=00-10");
		result &= table.AllocateRegionToOpcode(this, L"1110011111DDDDDD", L"DDDDDD=010000-110111,111000,111001");
		return result;
	}

	virtual Disassembly M68000Disassemble()
	{
		if(target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT)
		{
			return Disassembly(L"ROL." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
		}
		else
		{
			return Disassembly(L"ROL", target.Disassemble());
		}
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	In the case of the rotating of a register:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|---|---|---|-----------|
//	| 1 | 1 | 1 | 0 |  NUMBER/  | 1 |  SIZE |i/r| 1 | 1 | REGISTER  |
//	|   |   |   |   |  REGISTER |   |       |   |   |   |           |
//	-----------------------------------------------------------------
		if(data.GetDataSegment(6, 2) != 3)
		{
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

			target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			if(!data.GetBit(5))
			{
				source.BuildQuickData(location + GetInstructionSize(), data.GetDataSegment(9, 3));
			}
			else
			{
				source.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			}

			if(size == BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(6, 1, 0));
			}
		}
		else
//	In the case of the rotating of a memory area:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		{
			size = BITCOUNT_WORD;
			target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(target.ExtensionSize());
			source.BuildImmediateData(location + GetInstructionSize(), M68000Word(1));

			AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			AddExecuteCycleCount(target.DecodeTime());
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
		op1 %= 64;
		result = op2 << (op1 % op1.GetBitCount());
		result |= op2 >> (Data(size, op1.GetBitCount()) - (op1 % op1.GetBitCount()));
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		if(!op1.Zero())
		{
			cpu->SetC(op2.GetBit(op2.GetBitCount() - (op1 % op1.GetBitCount()).GetData()));
		}
		else
		{
			cpu->SetC(false);
		}

		//Calculate the additional execution time
		ExecuteTime additionalCycles;
		if(target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT)
		{
			additionalCycles.Set(2 * op1.GetData(), 0, 0);
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
