#include "M68000Instruction.h"
#ifndef __M68000_LEA_H__
#define __M68000_LEA_H__
namespace M68000 {

class LEA :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode _targetMode)
	{
		static const ExecuteTime executeTimeArray[9] = {
			//(An)                (An)+                -(An)                  d(An)                 d(An,ix)+              xxx.W                 xxx.L                  d(PC)                 d(PC,ix)
			ExecuteTime(4, 1, 0), ExecuteTime(0, 0, 0), ExecuteTime(0, 0, 0), ExecuteTime(8, 2, 0), ExecuteTime(12, 2, 0), ExecuteTime(8, 2, 0), ExecuteTime(12, 3, 0), ExecuteTime(8, 2, 0), ExecuteTime(12, 2, 0)};

		unsigned int _targetIndex = 0;
		switch (_targetMode)
		{
		case EffectiveAddress::Mode::AddRegIndirect:
			_targetIndex = 0;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPostInc:
			_targetIndex = 1;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPreDec:
			_targetIndex = 2;
			break;
		case EffectiveAddress::Mode::AddRegIndirectDisplace:
			_targetIndex = 3;
			break;
		case EffectiveAddress::Mode::AddRegIndirectIndex8Bit:
			_targetIndex = 4;
			break;
		case EffectiveAddress::Mode::ABSWord:
			_targetIndex = 5;
			break;
		case EffectiveAddress::Mode::ABSLong:
			_targetIndex = 6;
			break;
		case EffectiveAddress::Mode::PCIndirectDisplace:
			_targetIndex = 7;
			break;
		case EffectiveAddress::Mode::PCIndirectIndex8Bit:
			_targetIndex = 8;
			break;
		}
		return executeTimeArray[_targetIndex];
	}

	virtual LEA* Clone() const {return new LEA();}
	virtual LEA* ClonePlacement(void* buffer) const {return new(buffer) LEA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100***111DDDDDD", L"DDDDDD=010000-010111,101000-110111,111000,111001,111010,111011");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LEA";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | REGISTER  | 1 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		_source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_LONG, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_source.ExtensionSize());
		_target.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		AddExecuteCycleCount(GetExecuteTime(_source.GetAddressMode()));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long result;

		// If the _target location was read from a memory address, and the read value hasn't
		// been modified up to this point, record the _source memory address as a pointer to
		// code for disassembly purposes.
		unsigned int _targetReadFromAddress;
		bool dataIsOffset;
		unsigned int offsetBaseAddress;
		unsigned int dataSize;
		bool dataUnmodified = _target.IsTargetUnmodifiedFromMemoryReadV2(cpu, _targetReadFromAddress, dataIsOffset, offsetBaseAddress, dataSize);
		//##FIX## Clean all this junk up
		dataUnmodified &= !dataIsOffset;

		// Perform the operation
		_source.GetAddress(cpu, result);
		additionalTime += _target.Write(cpu, result, GetInstructionRegister(), false, false, dataUnmodified, _targetReadFromAddress, dataSize);

		// Adjust the PC and return the execution time
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
};

} // Close namespace M68000
#endif
