#include "M68000Instruction.h"
#ifndef __M68000_JSR_H__
#define __M68000_JSR_H__
namespace M68000 {

class JSR :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode _targetMode)
	{
		const ExecuteTime executeTimeArray[9] = {
			//(An)					(An)+					-(An)					d(An)					d(An,ix)+				xxx.W					xxx.L					d(PC)					d(PC,ix)
			ExecuteTime(16, 2, 2),	ExecuteTime(0, 0, 0),	ExecuteTime(0, 0, 0),	ExecuteTime(18, 2, 2),	ExecuteTime(22, 2, 2),	ExecuteTime(18, 2, 2),	ExecuteTime(20, 3, 2),	ExecuteTime(18, 2, 2),	ExecuteTime(22, 2, 2)};
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

	virtual JSR* Clone() const {return new JSR();}
	virtual JSR* ClonePlacement(void* buffer) const {return new(buffer) JSR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100111010DDDDDD", L"DDDDDD=010000-010111,101000-110111,111000,111001,111010,111011");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"JSR";
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
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 | 0 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|

		// JSR	<ea>
		_target.BuildAddressPredec(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);

		_source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_LONG, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_source.ExtensionSize());
		AddExecuteCycleCount(GetExecuteTime(_source.GetAddressMode()));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long address;

		//##FIX## If a data register is unmodified, and is then used as the index into a
		// branch, eg: "jsr off_468(pc,d0.w)", we need to mark the _source location for the
		// data register value as an offset.
		// If the _target location was read from a memory address, and the read value hasn't
		// been modified up to this point, record the _source memory address as a pointer to
		// code for disassembly purposes.
		unsigned int _targetReadFromAddress;
		bool dataIsOffset;
		unsigned int offsetBaseAddress;
		unsigned int dataSize;
		if (_source.IsTargetUnmodifiedFromMemoryReadV2(cpu, _targetReadFromAddress, dataIsOffset, offsetBaseAddress, dataSize))
		{
			cpu->AddDisassemblyAddressInfoOffset(_targetReadFromAddress, dataSize, true, dataIsOffset, offsetBaseAddress);
		}

		// Perform the operation
		_source.GetAddress(cpu, address);
		additionalTime += _target.Write(cpu, M68000Long(location + GetInstructionSize()), GetInstructionRegister());
		cpu->PushCallStack(cpu->GetPC().GetData(), address.GetData(), (location + GetInstructionSize()).GetData(), L"JSR");
		cpu->SetPC(address);

		// Detect possible jump tables for active disassembly
		if (cpu->ActiveDisassemblyEnabled() && ((_source.GetAddressMode() == EffectiveAddress::Mode::AddRegIndirectIndex8Bit) || (_source.GetAddressMode() == EffectiveAddress::Mode::PCIndirectIndex8Bit)))
		{
			M68000Long baseAddress;
			_source.GetAddressDisplacementTargetNoIndex(cpu, baseAddress);
			cpu->AddDisassemblyPossibleBranchTable(baseAddress.GetData(), address.GetData(), GetInstructionSize());
		}

		// Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetResultantPCLocations(std::set<unsigned int>& resultantPCLocations, bool& undeterminedResultantPCLocation) const
	{
		// Return the address directly after this opcode, and the jump location from
		// executing this opcode, if it can be determined statically, as the possible
		// resultant PC locations from executing this opcode. If the jump location cannot
		// be determined at this time because it relies on register contents, flag that
		// there is an undetermined resultant PC location from executing this opcode.
		undeterminedResultantPCLocation = false;
		unsigned int nextOpcodeAddress = GetInstructionLocation().GetData() + GetInstructionSize();
		resultantPCLocations.insert(nextOpcodeAddress);
		M68000Long jumpOpcodeAddress;
		if (_source.GetAddressTransparent(jumpOpcodeAddress))
		{
			resultantPCLocations.insert(jumpOpcodeAddress.GetData());
		}
		else
		{
			undeterminedResultantPCLocation = true;
		}
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
