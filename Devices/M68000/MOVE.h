#include "M68000Instruction.h"
#ifndef __M68000_MOVE_H__
#define __M68000_MOVE_H__
namespace M68000 {

class MOVE :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode _sourceMode, EffectiveAddress::Mode _targetMode, Bitcount operationSize)
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

		unsigned int _sourceIndex = 0;
		unsigned int _targetIndex = 0;
		switch (_sourceMode)
		{
		case EffectiveAddress::Mode::DataRegDirect:
			_sourceIndex = 0;
			break;
		case EffectiveAddress::Mode::AddRegDirect:
			_sourceIndex = 1;
			break;
		case EffectiveAddress::Mode::AddRegIndirect:
			_sourceIndex = 2;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPostInc:
			_sourceIndex = 3;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPreDec:
			_sourceIndex = 4;
			break;
		case EffectiveAddress::Mode::AddRegIndirectDisplace:
			_sourceIndex = 5;
			break;
		case EffectiveAddress::Mode::AddRegIndirectIndex8Bit:
			_sourceIndex = 6;
			break;
		case EffectiveAddress::Mode::ABSWord:
			_sourceIndex = 7;
			break;
		case EffectiveAddress::Mode::ABSLong:
			_sourceIndex = 8;
			break;
		case EffectiveAddress::Mode::PCIndirectDisplace:
			_sourceIndex = 9;
			break;
		case EffectiveAddress::Mode::PCIndirectIndex8Bit:
			_sourceIndex = 10;
			break;
		case EffectiveAddress::Mode::Immediate:
			_sourceIndex = 11;
			break;
		}
		switch (_targetMode)
		{
		case EffectiveAddress::Mode::DataRegDirect:
			_targetIndex = 0;
			break;
		case EffectiveAddress::Mode::AddRegDirect:
			_targetIndex = 1;
			break;
		case EffectiveAddress::Mode::AddRegIndirect:
			_targetIndex = 2;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPostInc:
			_targetIndex = 3;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPreDec:
			_targetIndex = 4;
			break;
		case EffectiveAddress::Mode::AddRegIndirectDisplace:
			_targetIndex = 5;
			break;
		case EffectiveAddress::Mode::AddRegIndirectIndex8Bit:
			_targetIndex = 6;
			break;
		case EffectiveAddress::Mode::ABSWord:
			_targetIndex = 7;
			break;
		case EffectiveAddress::Mode::ABSLong:
			_targetIndex = 8;
			break;
		}
		if (operationSize != BITCOUNT_LONG)
		{
			return executeTimeArray[_sourceIndex][_targetIndex];
		}
		else
		{
			return executeTimeArrayLong[_sourceIndex][_targetIndex];
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
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|-------|-----------|-----------|-----------|-----------|
//	| 0 | 0 |  SIZE |  REGISTER |    MODE   |    MODE   | REGISTER  |
//	----------------************************=========================
//	                |----destination <ea>---|------_source <ea>------|
		switch (data.GetDataSegment(12, 2))
		{
		case 1:	//01
			_size = BITCOUNT_BYTE;
			break;
		case 3:	//11
			_size = BITCOUNT_WORD;
			break;
		case 2:	//10
			_size = BITCOUNT_LONG;
			break;
		}

		_source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_source.ExtensionSize());
		_target.Decode(data.GetDataSegment(9, 3), data.GetDataSegment(6, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_target.ExtensionSize());
		AddExecuteCycleCount(GetExecuteTime(_source.GetAddressMode(), _target.GetAddressMode(), _size));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data result(_size);

		//Read information about the _source for use in active disassembly
		unsigned int _sourceReadFromAddress;
		bool _sourceIsUnmodifiedFromAddress = _source.IsTargetUnmodifiedFromMemoryRead(cpu, _size, _sourceReadFromAddress);
		unsigned int _sourceReadFromAddressSize = _source.GetTargetOriginalMemoryReadSize(cpu, _size);

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

		//##TODO## Check what happens when the _source address is pre-decremented as well.
		//Does it actually read one word, write one word, read another word, and write
		//another word, or does it read both words first, then write both words. Try a
		//sequence like the following to test:
		//lea		#$C0000C, A0
		//lea		#$FF0004, A1
		//move.l	-(A0),-(A1)
		//The HV counter will increment between the reads, which will allow the order the
		//words are read in to be determined.
		additionalTime += _source.Read(cpu, result, GetInstructionRegister());
		if ((_target.GetAddressMode() == EffectiveAddress::Mode::AddRegIndirectPreDec) && (_size == BITCOUNT_LONG))
		{
			M68000Word lower(result.GetLowerBits(BITCOUNT_WORD));
			M68000Word upper(result.GetUpperBits(BITCOUNT_WORD));
			additionalTime += _target.Write(cpu, lower, GetInstructionRegister());
			additionalTime += _target.Write(cpu, upper, GetInstructionRegister());
		}
		else
		{
			additionalTime += _target.Write(cpu, result, GetInstructionRegister(), false, false, _sourceIsUnmodifiedFromAddress, _sourceReadFromAddress, _sourceReadFromAddressSize);
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
		_source.AddLabelTargetsToSet(labelTargetLocations);
		_target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
	Bitcount _size;
};

} //Close namespace M68000
#endif
