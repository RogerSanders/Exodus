#include "M68000Instruction.h"
#ifndef __M68000_CMPI_H__
#define __M68000_CMPI_H__
namespace M68000 {

class CMPI :public M68000Instruction
{
public:
	virtual CMPI* Clone() const {return new CMPI();}
	virtual CMPI* ClonePlacement(void* buffer) const {return new(buffer) CMPI();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00001100CCDDDDDD", L"CC=00-10 DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"CMPI";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	                                        |----------<ea>---------|
//	----------------------------------------=========================
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|-------|-----------|-----------|
//	| 0 | 0 | 0 | 0 | 1 | 1 | 0 | 0 | SIZE  |    MODE   | REGISTER  |
//	|-------------------------------|-------------------------------|
//	|         16 BITS DATA          |         8 BITS DATA           |
//	|---------------------------------------------------------------|
//	|                          32 BITS DATA                         |
//	-----------------------------------------------------------------
		switch (data.GetDataSegment(6, 2))
		{
		case 0:	// 00
			_size = BITCOUNT_BYTE;
			break;
		case 1:	// 01
			_size = BITCOUNT_WORD;
			break;
		case 2:	// 10
			_size = BITCOUNT_LONG;
			break;
		}

		// SUBI	#<data>,<ea>
		_source.BuildImmediateData(_size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_source.ExtensionSize());
		_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_target.ExtensionSize());

		if (_target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect)
		{
			if (_size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 2, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(14, 3, 0));
			}
		}
		else
		{
			if (_size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 2, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(12, 3, 0));
			}
			AddExecuteCycleCount(_target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(_size);
		Data op2(_size);
		Data result(_size);

		// Perform the operation
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.Read(cpu, op2, GetInstructionRegister());
		result = op2 - op1;

		// Set the flag results
		bool overflow = (op1.MSB() == result.MSB()) && (op2.MSB() != op1.MSB());
		bool borrow = (op1.MSB() && result.MSB()) || (!op2.MSB() && (op1.MSB() || result.MSB()));
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(overflow);
		cpu->SetC(borrow);

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
