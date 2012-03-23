#include "M68000Instruction.h"
#ifndef __M68000_CMP_H__
#define __M68000_CMP_H__
namespace M68000 {

class CMP :public M68000Instruction
{
public:
	virtual CMP* Clone() const {return new CMP();}
	virtual CMP* ClonePlacement(void* buffer) const {return new(buffer) CMP();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1011***0CCDDDDDD", L"CC=00 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"1011***0CCDDDDDD", L"CC=01-10 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"CMP";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|-----------|-----------|
//	| 1 | 0 | 1 | 1 |  REGISTER | 0 | SIZE  |    MODE   |  REGISTER |
//	----------------------------------------=========================
//                              |--<opmode>-|----------<ea>---------|
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

		//CMP	<ea>,Dn
		target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());

		if(size == BITCOUNT_LONG)
		{
			AddExecuteCycleCount(ExecuteTime(6, 1, 0));
		}
		else
		{
			AddExecuteCycleCount(ExecuteTime(4, 1, 0));
		}
		AddExecuteCycleCount(source.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size);
		Data op2(size);
		Data result(size);

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.Read(cpu, op2, GetInstructionRegister());
		result = op2 - op1;

		//Set the flag results
		bool overflow = (op1.MSB() == result.MSB()) && (op2.MSB() != op1.MSB());
		bool borrow = (op1.MSB() && result.MSB()) || (!op2.MSB() && (op1.MSB() || result.MSB()));
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(overflow);
		cpu->SetC(borrow);

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
