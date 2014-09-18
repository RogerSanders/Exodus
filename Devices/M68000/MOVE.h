#include "M68000Instruction.h"
#ifndef __M68000_MOVE_H__
#define __M68000_MOVE_H__
namespace M68000 {

class MOVE :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode sourceMode, EffectiveAddress::Mode targetMode, Bitcount size)
	{
		static const ExecuteTime executeTimeArray[12][9] = {
			           //Dn                     An                     (An)                   (An)+                  -(An)                  d(An)                  d(An,ix)               xxx.W                  xxx.L
			/*Dn*/      {ExecuteTime(4, 1, 0),  ExecuteTime(4, 1, 0),  ExecuteTime(8, 1, 1),  ExecuteTime(8, 1, 1),  ExecuteTime(8, 1, 1),  ExecuteTime(12, 2, 1), ExecuteTime(14, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(16, 3, 1)},
			/*An*/      {ExecuteTime(4, 1, 0),  ExecuteTime(4, 1, 0),  ExecuteTime(8, 1, 1),  ExecuteTime(8, 1, 1),  ExecuteTime(8, 1, 1),  ExecuteTime(12, 2, 1), ExecuteTime(14, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(16, 3, 1)},
			/*(An)*/    {ExecuteTime(8, 2, 0),  ExecuteTime(8, 2, 0),  ExecuteTime(12, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(16, 3, 1), ExecuteTime(18, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(20, 4, 1)},
			/*(An)+*/   {ExecuteTime(8, 2, 0),  ExecuteTime(8, 2, 0),  ExecuteTime(12, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(16, 3, 1), ExecuteTime(18, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(20, 4, 1)},
			/*-(An)*/   {ExecuteTime(10, 2, 0), ExecuteTime(10, 2, 0), ExecuteTime(14, 2, 1), ExecuteTime(14, 2, 1), ExecuteTime(14, 2, 1), ExecuteTime(18, 3, 1), ExecuteTime(20, 4, 1), ExecuteTime(18, 3, 1), ExecuteTime(22, 4, 1)},
			/*d(An)*/   {ExecuteTime(12, 3, 0), ExecuteTime(12, 3, 0), ExecuteTime(16, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(20, 4, 1), ExecuteTime(22, 4, 1), ExecuteTime(20, 4, 1), ExecuteTime(24, 5, 1)},
			/*d(An,ix)*/{ExecuteTime(14, 3, 0), ExecuteTime(14, 3, 0), ExecuteTime(18, 3, 1), ExecuteTime(18, 3, 1), ExecuteTime(18, 3, 1), ExecuteTime(22, 4, 1), ExecuteTime(24, 4, 1), ExecuteTime(22, 4, 1), ExecuteTime(26, 5, 1)},
			/*xxx.W*/   {ExecuteTime(12, 3, 0), ExecuteTime(12, 3, 0), ExecuteTime(16, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(20, 4, 1), ExecuteTime(22, 4, 1), ExecuteTime(20, 4, 1), ExecuteTime(24, 5, 1)},
			/*xxx.L*/   {ExecuteTime(16, 4, 0), ExecuteTime(16, 4, 0), ExecuteTime(20, 4, 1), ExecuteTime(20, 4, 1), ExecuteTime(20, 4, 1), ExecuteTime(24, 5, 1), ExecuteTime(26, 5, 1), ExecuteTime(24, 5, 1), ExecuteTime(28, 6, 1)},
			/*d(PC)*/   {ExecuteTime(12, 3, 0), ExecuteTime(12, 3, 0), ExecuteTime(16, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(20, 4, 1), ExecuteTime(22, 4, 1), ExecuteTime(20, 4, 1), ExecuteTime(24, 5, 1)},
			/*d(PC,ix)*/{ExecuteTime(14, 3, 0), ExecuteTime(14, 3, 0), ExecuteTime(18, 3, 1), ExecuteTime(18, 3, 1), ExecuteTime(18, 3, 1), ExecuteTime(22, 4, 1), ExecuteTime(24, 4, 1), ExecuteTime(22, 4, 1), ExecuteTime(26, 5, 1)},
			/*#xxx*/    {ExecuteTime(8, 2, 0),  ExecuteTime(8, 2, 0),  ExecuteTime(12, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(12, 2, 1), ExecuteTime(16, 3, 1), ExecuteTime(18, 3, 1), ExecuteTime(16, 3, 1), ExecuteTime(20, 4, 1)}};
		static const ExecuteTime executeTimeArrayLong[12][9] = {
			           //Dn                     An                     (An)                   (An)+                  -(An)                  d(An)                  d(An,ix)               xxx.W                  xxx.L
			/*Dn*/      {ExecuteTime(4, 1, 0),  ExecuteTime(4, 1, 0),  ExecuteTime(12, 1, 2), ExecuteTime(12, 1, 2), ExecuteTime(12, 1, 2), ExecuteTime(16, 2, 2), ExecuteTime(18, 2, 2), ExecuteTime(16, 2, 2), ExecuteTime(20, 3, 2)},
			/*An*/      {ExecuteTime(4, 1, 0),  ExecuteTime(4, 1, 0),  ExecuteTime(12, 1, 2), ExecuteTime(12, 1, 2), ExecuteTime(12, 1, 2), ExecuteTime(16, 2, 2), ExecuteTime(18, 2, 2), ExecuteTime(16, 2, 2), ExecuteTime(20, 3, 2)},
			/*(An)*/    {ExecuteTime(12, 3, 0), ExecuteTime(12, 3, 0), ExecuteTime(20, 3, 2), ExecuteTime(20, 3, 2), ExecuteTime(20, 3, 2), ExecuteTime(24, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(28, 5, 2)},
			/*(An)+*/   {ExecuteTime(12, 3, 0), ExecuteTime(12, 3, 0), ExecuteTime(20, 3, 2), ExecuteTime(20, 3, 2), ExecuteTime(20, 3, 2), ExecuteTime(24, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(28, 5, 2)},
			/*-(An)*/   {ExecuteTime(14, 3, 0), ExecuteTime(14, 3, 0), ExecuteTime(22, 3, 2), ExecuteTime(22, 3, 2), ExecuteTime(22, 3, 2), ExecuteTime(26, 4, 2), ExecuteTime(28, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(30, 5, 2)},
			/*d(An)*/   {ExecuteTime(16, 4, 0), ExecuteTime(16, 4, 0), ExecuteTime(24, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(28, 5, 2), ExecuteTime(30, 5, 2), ExecuteTime(28, 5, 2), ExecuteTime(32, 6, 2)},
			/*d(An,ix)*/{ExecuteTime(18, 4, 0), ExecuteTime(18, 4, 0), ExecuteTime(26, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(30, 5, 2), ExecuteTime(32, 5, 2), ExecuteTime(30, 5, 2), ExecuteTime(34, 6, 2)},
			/*xxx.W*/   {ExecuteTime(16, 4, 0), ExecuteTime(16, 4, 0), ExecuteTime(24, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(28, 5, 2), ExecuteTime(30, 5, 2), ExecuteTime(28, 5, 2), ExecuteTime(32, 6, 2)},
			/*xxx.L*/   {ExecuteTime(20, 5, 0), ExecuteTime(20, 5, 0), ExecuteTime(28, 5, 2), ExecuteTime(28, 5, 2), ExecuteTime(28, 5, 2), ExecuteTime(32, 6, 2), ExecuteTime(34, 6, 2), ExecuteTime(32, 6, 2), ExecuteTime(36, 7, 2)},
			/*d(PC)*/   {ExecuteTime(16, 4, 0), ExecuteTime(16, 4, 0), ExecuteTime(24, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(28, 5, 2), ExecuteTime(30, 5, 2), ExecuteTime(28, 5, 2), ExecuteTime(32, 5, 2)},
			/*d(PC,ix)*/{ExecuteTime(18, 4, 0), ExecuteTime(18, 4, 0), ExecuteTime(26, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(30, 5, 2), ExecuteTime(32, 5, 2), ExecuteTime(30, 5, 2), ExecuteTime(34, 6, 2)},
			/*#xxx*/    {ExecuteTime(12, 3, 0), ExecuteTime(12, 3, 0), ExecuteTime(20, 3, 2), ExecuteTime(20, 3, 2), ExecuteTime(20, 3, 2), ExecuteTime(24, 4, 2), ExecuteTime(26, 4, 2), ExecuteTime(24, 4, 2), ExecuteTime(28, 5, 2)}};

		unsigned int sourceIndex = 0;
		unsigned int targetIndex = 0;
		switch(sourceMode)
		{
		case EffectiveAddress::DATAREG_DIRECT:
			sourceIndex = 0;
			break;
		case EffectiveAddress::ADDREG_DIRECT:
			sourceIndex = 1;
			break;
		case EffectiveAddress::ADDREG_INDIRECT:
			sourceIndex = 2;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_POSTINC:
			sourceIndex = 3;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_PREDEC:
			sourceIndex = 4;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_DISPLACE:
			sourceIndex = 5;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_INDEX_8BIT:
			sourceIndex = 6;
			break;
		case EffectiveAddress::ABS_WORD:
			sourceIndex = 7;
			break;
		case EffectiveAddress::ABS_LONG:
			sourceIndex = 8;
			break;
		case EffectiveAddress::PC_INDIRECT_DISPLACE:
			sourceIndex = 9;
			break;
		case EffectiveAddress::PC_INDIRECT_INDEX_8BIT:
			sourceIndex = 10;
			break;
		case EffectiveAddress::IMMEDIATE:
			sourceIndex = 11;
			break;
		}
		switch(targetMode)
		{
		case EffectiveAddress::DATAREG_DIRECT:
			targetIndex = 0;
			break;
		case EffectiveAddress::ADDREG_DIRECT:
			targetIndex = 1;
			break;
		case EffectiveAddress::ADDREG_INDIRECT:
			targetIndex = 2;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_POSTINC:
			targetIndex = 3;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_PREDEC:
			targetIndex = 4;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_DISPLACE:
			targetIndex = 5;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_INDEX_8BIT:
			targetIndex = 6;
			break;
		case EffectiveAddress::ABS_WORD:
			targetIndex = 7;
			break;
		case EffectiveAddress::ABS_LONG:
			targetIndex = 8;
			break;
		}
		if(size != BITCOUNT_LONG)
		{
			return executeTimeArray[sourceIndex][targetIndex];
		}
		else
		{
			return executeTimeArrayLong[sourceIndex][targetIndex];
		}
	}

	virtual MOVE* Clone() const {return new MOVE();}
	virtual MOVE* ClonePlacement(void* buffer) const {return new(buffer) MOVE();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"00CC***DDDEEEEEE", L"CC=01 DDD=000,010,011,100,101,110 EEEEEE=000000-000111,010000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"00CCDDDDDDEEEEEE", L"CC=01 DDDDDD=000111,001111 EEEEEE=000000-000111,010000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"00CC***DDDEEEEEE", L"CC=11,10 DDD=000,010,011,100,101,110 EEEEEE=000000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"00CCDDDDDDEEEEEE", L"CC=11,10 DDDDDD=000111,001111 EEEEEE=000000-110111,111000,111001,111010,111011,111100");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MOVE";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(size), source.Disassemble(labelSettings) + L", " + target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|-------|-----------|-----------|-----------|-----------|
//	| 0 | 0 |  SIZE |  REGISTER |    MODE   |    MODE   | REGISTER  |
//	----------------************************=========================
//	                |----destination <ea>---|------source <ea>------|
		switch(data.GetDataSegment(12, 2))
		{
		case 1:	//01
			size = BITCOUNT_BYTE;
			break;
		case 3:	//11
			size = BITCOUNT_WORD;
			break;
		case 2:	//10
			size = BITCOUNT_LONG;
			break;
		}

		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		target.Decode(data.GetDataSegment(9, 3), data.GetDataSegment(6, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(GetExecuteTime(source.GetAddressMode(), target.GetAddressMode(), size));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data result(size);

		//Read information about the source for use in active disassembly
		unsigned int sourceReadFromAddress;
		bool sourceIsUnmodifiedFromAddress = source.IsTargetUnmodifiedFromMemoryRead(cpu, size, sourceReadFromAddress);
		unsigned int sourceReadFromAddressSize = source.GetTargetOriginalMemoryReadSize(cpu, size);

		//Perform the operation
		//##NOTE## We need to break long writes into two separate word writes here when
		//the destination address is pre-decremented. Long word writes are normally
		//broken up into two separate word writes and we emulate that behaviour, however
		//due to an undocumented design quirk, when the destination address is using
		//predec mode in this one instruction, the bytes are written in reverse order.
		//For other instructions when writing a long of 0x12345678 to a predecremented
		//address, the writes would occur in the following order:
		//-Write 0x1234 to (address - 4)
		//-Write 0x5678 to (address - 2)
		//For a move.l however, the results are as follows:
		//-Write 0x5678 to (address - 2)
		//-Write 0x1234 to (address - 4)
		//The result is the same when writing to a simple memory buffer, but the order
		//writes occur in can make a difference when interacting with devices rather than
		//just memory buffers. Refer to "68000 Undocumented Behavior Notes" by Bart
		//Trzynadlowski for more info.

		//##TODO## Check what happens when the source address is pre-decremented as well.
		//Does it actually read one word, write one word, read another word, and write
		//another word, or does it read both words first, then write both words. Try a
		//sequence like the following to test:
		//lea		#$C0000C, A0
		//lea		#$FF0004, A1
		//move.l	-(A0),-(A1)
		//The HV counter will increment between the reads, which will allow the order the
		//words are read in to be determined.
		additionalTime += source.Read(cpu, result, GetInstructionRegister());
		if((target.GetAddressMode() == EffectiveAddress::ADDREG_INDIRECT_PREDEC) && (size == BITCOUNT_LONG))
		{
			M68000Word lower(result.GetLowerBits(BITCOUNT_WORD));
			M68000Word upper(result.GetUpperBits(BITCOUNT_WORD));
			additionalTime += target.Write(cpu, lower, GetInstructionRegister());
			additionalTime += target.Write(cpu, upper, GetInstructionRegister());
		}
		else
		{
			additionalTime += target.Write(cpu, result, GetInstructionRegister(), false, false, sourceIsUnmodifiedFromAddress, sourceReadFromAddress, sourceReadFromAddressSize);
		}

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		cpu->SetC(false);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		source.AddLabelTargetsToSet(labelTargetLocations);
		target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
