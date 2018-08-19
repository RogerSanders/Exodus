#include "M68000Instruction.h"
#ifndef __M68000_BRA_H__
#define __M68000_BRA_H__
namespace M68000 {

class BRA :public M68000Instruction
{
public:
	virtual BRA* Clone() const {return new BRA();}
	virtual BRA* ClonePlacement(void* buffer) const {return new(buffer) BRA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01100000********", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"BRA";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		if(labelSettings.enableSubstitution)
		{
			return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _target.DisassembleImmediateAsPCDisplacement(labelSettings));
		}
		else
		{
			return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _target.DisassembleImmediateAsPCDisplacement(labelSettings), _target.DisassembleImmediateAsPCDisplacementTargetAddressString());
		}
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|-------------------------------|
//	| 0 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |         8 BITS OFFSET         |
//	|---------------------------------------------------------------|
//	|            16 BITS OFFSET, IF 8 BITS OFFSET = $00             |
//	-----------------------------------------------------------------

		//BRA	<label>
		if(data.GetDataSegment(0, 8) != 0)
		{
			_size = BITCOUNT_BYTE;
			_target.BuildImmediateData(location + GetInstructionSize(), M68000Byte(data.GetDataSegment(0, 8)));
		}
		else
		{
			_size = BITCOUNT_WORD;
			_target.BuildImmediateData(_size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(_target.ExtensionSize());
		}
		AddExecuteCycleCount(ExecuteTime(10, 2, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long newPC;

		//Return the execution time
		Data offset(_size);
		additionalTime += _target.Read(cpu, offset, GetInstructionRegister());
		newPC = _target.GetSavedPC() + M68000Long(offset.SignExtend(BITCOUNT_LONG));
		cpu->SetPC(newPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetResultantPCLocations(std::set<unsigned int>& resultantPCLocations, bool& undeterminedResultantPCLocation) const
	{
		//Return the branch location from executing this opcode as the only possible
		//resultant PC location from executing this opcode.
		undeterminedResultantPCLocation = false;
		unsigned int branchOpcodeAddress = (_target.GetSavedPC() + _target.ExtractProcessedImmediateData()).GetData();
		resultantPCLocations.insert(branchOpcodeAddress);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		unsigned int branchOpcodeAddress = (_target.GetSavedPC() + _target.ExtractProcessedImmediateData()).GetData();
		labelTargetLocations.insert(branchOpcodeAddress);
	}

private:
	Bitcount _size;
	EffectiveAddress _target;
};

} //Close namespace M68000
#endif
