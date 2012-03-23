#include "M68000Instruction.h"
#ifndef __M68000_MULU_H__
#define __M68000_MULU_H__
namespace M68000 {

class MULU :public M68000Instruction
{
public:
	virtual MULU* Clone() const {return new MULU();}
	virtual MULU* ClonePlacement(void* buffer) const {return new(buffer) MULU();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"1100***011DDDDDD", L"DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MULU";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(BITCOUNT_WORD), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|---|---|-----------|-----------|
//	| 1 | 1 | 0 | 0 | REGISTER  | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|

		//MULU.W	<ea>,Dn		16*16->32
		size = BITCOUNT_WORD;
		target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());

		AddExecuteCycleCount(ExecuteTime(38, 1, 0));
		AddExecuteCycleCount(source.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data temp1(size);
		Data temp2(size);
		M68000Long op1;
		M68000Long op2;
		M68000Long result;

		//Perform the operation
		additionalTime += source.Read(cpu, temp1, GetInstructionRegister());
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, temp2, GetInstructionRegister());
		op1 = M68000Long(temp1.Convert(BITCOUNT_LONG));
		op2 = M68000Long(temp2.Convert(BITCOUNT_LONG));
		result = op1 * op2;
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		cpu->SetC(false);

		//Calculate the additional execution time. According to the M68000 Users Manual,
		//section 8.3:
		//MULS, MULU — The multiply algorithm requires 38+2n clocks where n is defined as
		//	MULU: n = the number of ones in the <ea>
		//	MULS: n=concatenate the <ea> with a zero as the LSB; n is the resultant
		//	number of 10 or 01 patterns in the 17-bit source; i.e., worst case happens
		//	when the source is $5555.
		ExecuteTime additionalCycles;
		additionalCycles.cycles = 2 * op1.GetSetBitCount();

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	Bitcount size;
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
