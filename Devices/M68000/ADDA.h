#include "M68000Instruction.h"
#ifndef __M68000_ADDA_H__
#define __M68000_ADDA_H__
namespace M68000 {

class ADDA :public M68000Instruction
{
public:
	virtual ADDA* Clone() const {return new ADDA();}
	virtual ADDA* ClonePlacement(void* buffer) const {return new(buffer) ADDA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"1101***CC1DDDDDD", L"CC=01,11 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ADDA";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|-------|---|-----------|-----------|
//	| 1 | 1 | 0 | 1 |  REGISTER |  SIZE | 1 |    MODE   |  REGISTER |
//	----------------------------------------=========================
//                              |---opmode--|----------<ea>---------|
		switch (data.GetDataSegment(7, 2))
		{
		case 1:	// 01
			_size = BITCOUNT_WORD;
			break;
		case 3:	// 11
			_size = BITCOUNT_LONG;
			break;
		}

		// ADDA	<ea>,An
		_source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_source.ExtensionSize());
		_target.BuildAddressDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));

		if (_size == BITCOUNT_LONG)
		{
			AddExecuteCycleCount(ExecuteTime(6, 1, 0));
			if ((_source.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect) || (_source.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect) || (_source.GetAddressMode() == EffectiveAddress::Mode::Immediate))
			{
				AddExecuteCycleCount(ExecuteTime(2, 0, 0));
			}
		}
		else
		{
			//##TODO## These times look a bit funny. Find a second _source to verify execution times against.
			AddExecuteCycleCount(ExecuteTime(8, 1, 0));
		}
		AddExecuteCycleCount(_source.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long op1;
		M68000Long op2;
		M68000Long result;
		Data temp(_size);

		// Perform the operation
		additionalTime += _source.Read(cpu, temp, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		op1 = M68000Long(temp.SignExtend(BITCOUNT_LONG));
		result = op2 + op1;
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
	Bitcount _size;
};

} // Close namespace M68000
#endif
