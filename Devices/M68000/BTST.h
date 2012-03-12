#include "M68000Instruction.h"
#ifndef __M68000_BTST_H__
#define __M68000_BTST_H__
namespace M68000 {

class BTST :public M68000Instruction
{
public:
	virtual BTST* Clone() {return new BTST();}
	virtual BTST* ClonePlacement(void* buffer) {return new(buffer) BTST();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		bool result = true;
		result &= table->AllocateRegionToOpcode(this, L"0000***B00DDDDDD", L"B=1 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
		result &= table->AllocateRegionToOpcode(this, L"0000100B00DDDDDD", L"B=0 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011");
		return result;
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"BTST." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
		size = (data.GetDataSegment(3, 3) == 0)? BITCOUNT_LONG: BITCOUNT_BYTE;
		if(data.GetBit(8))
		{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|---|---|-----------|-----------|
//	| 0 | 0 | 0 | 0 |  REGISTER | 1 | 0 | 0 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
			source.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(target.ExtensionSize());
			if((target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT) || (target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT))
			{
				AddExecuteCycleCount(ExecuteTime(6, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
		}
		else
		{
//	                                        |----------<ea>---------|
//	----------------------------------------=========================
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |    MODE   | REGISTER  |
//	|---|---|---|---|---|---|---|---|-------------------------------|
//	| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       NUMBER OF THE BIT       |
//	-----------------------------------------------------------------

			source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(source.ExtensionSize());
			target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(target.ExtensionSize());
			if((target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT) || (target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT))
			{
				AddExecuteCycleCount(ExecuteTime(10, 2, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 2, 0));
			}
		}
		AddExecuteCycleCount(target.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Byte op1;
		Data op2(size);

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.Read(cpu, op2, GetInstructionRegister());
		unsigned int targetBit = (op1.GetData() % op2.GetBitCount());

		//Set the flag results
		cpu->SetZ(!op2.GetBit(targetBit));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	Bitcount size;
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
