#include "M68000Instruction.h"
#ifndef __M68000_PEA_H__
#define __M68000_PEA_H__
namespace M68000 {

class PEA :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode _targetMode)
	{
		const ExecuteTime executeTimeArray[9] = {
			//(An)					(An)+					-(An)					d(An)					d(An,ix)+				xxx.W					xxx.L					d(PC)					d(PC,ix)
			ExecuteTime(12, 2, 1),	ExecuteTime(0, 0, 0),	ExecuteTime(0, 0, 0),	ExecuteTime(16, 2, 2),	ExecuteTime(20, 2, 2),	ExecuteTime(16, 2, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(16, 2, 2),	ExecuteTime(20, 2, 2)};
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

	virtual PEA* Clone() const {return new PEA();}
	virtual PEA* ClonePlacement(void* buffer) const {return new(buffer) PEA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100100001DDDDDD", L"DDDDDD=010000-010111,101000-110111,111000,111001,111010,111011");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"PEA";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _source.Disassemble(labelSettings));
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 0 | 0 | 0 | 0 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		_source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_LONG, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_source.ExtensionSize());
		_target.BuildAddressPredec(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		AddExecuteCycleCount(GetExecuteTime(_source.GetAddressMode()));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long result;

		// Perform the operation
		_source.GetAddress(cpu, result);
		additionalTime += _target.Write(cpu, result, GetInstructionRegister());

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
