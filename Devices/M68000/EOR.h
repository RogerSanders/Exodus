#include "M68000Instruction.h"
#ifndef __M68000_EOR_H__
#define __M68000_EOR_H__
namespace M68000 {

class EOR :public M68000Instruction
{
public:
	virtual EOR* Clone() const {return new EOR();}
	virtual EOR* ClonePlacement(void* buffer) const {return new(buffer) EOR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"1011***1CCDDDDDD", L"CC=00-10 DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"EOR";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|-----------|-----------|
//	| 1 | 0 | 1 | 1 |  REGISTER | 1 |  SIZE |    MODE   |  REGISTER |
//	----------------------------=====================================
//	                            |--<opmode>-|----------<ea>---------|
		switch (data.GetDataSegment(6, 2))
		{
		case 0:	//00
			_size = BITCOUNT_BYTE;
			break;
		case 1:	//01
			_size = BITCOUNT_WORD;
			break;
		case 2:	//10
			_size = BITCOUNT_LONG;
			break;
		}

		//EOR	Dn,<ea>
		_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_target.ExtensionSize());
		_source.BuildDataDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));

		if (_size == BITCOUNT_LONG)
		{
			if (_source.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect)
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(12, 1, 2));
			}
		}
		else
		{
			if (_source.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect)
			{
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			}
		}
		AddExecuteCycleCount(_target.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(_size);
		Data op2(_size);
		Data result(_size);

		//Perform the operation
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		result = op2 ^ op1;
		additionalTime += _target.Write(cpu, result, GetInstructionRegister());

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
