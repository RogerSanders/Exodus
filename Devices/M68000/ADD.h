#include "M68000Instruction.h"
#ifndef __M68000_ADD_H__
#define __M68000_ADD_H__
namespace M68000 {

class ADD :public M68000Instruction
{
public:
	virtual ADD* Clone() const {return new ADD();}
	virtual ADD* ClonePlacement(void* buffer) const {return new(buffer) ADD();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1101***BCCDDDDDD", L"B=0 CC=00 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"1101***BCCDDDDDD", L"B=0 CC=01-10 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"1101***BCCDDDDDD", L"B=1 CC=00-10 DDDDDD=010000-110111,111000,111001");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ADD";
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
//	| 1 | 1 | 0 | 1 |  REGISTER | D | SIZE  |    MODE   |  REGISTER |
//	----------------------------=====================================
//	                            |--<opmode>-|----------<ea>---------|
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

		if(!data.GetBit(8))
		{
			//ADD	<ea>,Dn
			target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(source.ExtensionSize());

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
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
			AddExecuteCycleCount(source.DecodeTime());
		}
		else
		{
			//ADD	Dn,<ea>
			target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(target.ExtensionSize());
			source.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));

			if(size == BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(12, 1, 2));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			}
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
		result = op2 + op1;
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		bool carry = (op1.MSB() && op2.MSB()) || (!result.MSB() && (op1.MSB() || op2.MSB()));
		bool overflow = (op1.MSB() == op2.MSB()) && (result.MSB() != (op1.MSB() && op2.MSB()));
		cpu->SetX(carry);
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
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
