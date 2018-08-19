#include "M68000Instruction.h"
#ifndef __M68000_MOVEA_H__
#define __M68000_MOVEA_H__
namespace M68000 {

class MOVEA :public M68000Instruction
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
		switch(_sourceMode)
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
		switch(_targetMode)
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
		if(operationSize != BITCOUNT_LONG)
		{
			return executeTimeArray[_sourceIndex][_targetIndex];
		}
		else
		{
			return executeTimeArrayLong[_sourceIndex][_targetIndex];
		}
	}

	virtual MOVEA* Clone() const {return new MOVEA();}
	virtual MOVEA* ClonePlacement(void* buffer) const {return new(buffer) MOVEA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00CC***001DDDDDD", L"CC=11,10 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MOVEA";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|-------|-----------|---|---|---|-----------|-----------|
//	| 0 | 0 |  SIZE |  ADDRESS  | 0 | 0 | 1 |    MODE   | REGISTER  |
//	|   |   |       |  REGISTER |   |   |   |           |           |
//	----------------------------------------=========================
//	                                        |------_source <ea>------|
		switch(data.GetDataSegment(12, 2))
		{
		case 3:	//11
			_size = BITCOUNT_WORD;
			break;
		case 2:	//10
			_size = BITCOUNT_LONG;
			break;
		}

		_source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_source.ExtensionSize());
		_target.BuildAddressDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
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
		additionalTime += _source.Read(cpu, result, GetInstructionRegister());
		additionalTime += _target.Write(cpu, result.SignExtend(BITCOUNT_LONG), GetInstructionRegister(), false, false, _sourceIsUnmodifiedFromAddress, _sourceReadFromAddress, _sourceReadFromAddressSize);

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
