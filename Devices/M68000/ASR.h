#include "M68000Instruction.h"
#ifndef __M68000_ASR_H__
#define __M68000_ASR_H__
namespace M68000 {

class ASR :public M68000Instruction
{
public:
	virtual ASR* Clone() {return new ASR();}
	virtual ASR* ClonePlacement(void* buffer) {return new(buffer) ASR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1110***0CC*00***", L"CC=00-10");
		result &= table.AllocateRegionToOpcode(this, L"1110000011DDDDDD", L"DDDDDD=010000-110111,111000,111001");
		return result;
	}

	virtual Disassembly M68000Disassemble()
	{
		if(target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT)
		{
			return Disassembly(L"ASR." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
		}
		else
		{
			return Disassembly(L"ASR", target.Disassemble());
		}
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	In the case of the shifting of a register:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|---|---|---|-----------|
//	| 1 | 1 | 1 | 0 |  NUMBER/  | 0 |  SIZE |i/r| 0 | 0 | REGISTER  |
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
//	In the case of the shifting of a memory area:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                                  <ea>
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
		//##NOTE## Doing a shift on an x86 platform effectively does a modulus by 32 to
		//the shift count before performing the operation. Since the M68000 platform
		//works up to 64, we zero the target operand to simulate a shift greater than
		//31 places.
		if(op1.GetData() >= 32)
		{
			op2 = 0;
		}
		result = op2 >> op1;
		//Sign extend the result. What we do here is construct a temporary data object,
		//but we set the maximum bitcount of the temp data to the number of bits which
		//were NOT discarded in the source as a result of the shift. We can then sign
		//extend this data to get the final result.
		unsigned int remainingBitCount = 0;
		if(op1 < op2.GetBitCount())
		{
			remainingBitCount = op2.GetBitCount() - op1.GetData();
		}
		Data remainingBits(remainingBitCount, result.GetData());
		result = remainingBits.SignExtend(size);
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		//##NOTE## Even though the official documentation lumps this in with ASL and says
		//the V flag should be set if the MSB changes during the operation, an arithmetic
		//shift sign extends the result. Since we're shifting to the right, the MSB never
		//changes, so we simply need to clear this flag.
		cpu->SetV(false);
		if(!op1.Zero())
		{
			if(op1.GetData() > op2.GetBitCount())
			{
				cpu->SetX(op2.MSB());
				cpu->SetC(op2.MSB());
			}
			else
			{
				cpu->SetX(op2.GetBit(op1.GetData() - 1));
				cpu->SetC(op2.GetBit(op1.GetData() - 1));
			}
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
