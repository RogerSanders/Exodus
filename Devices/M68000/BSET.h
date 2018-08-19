#include "M68000Instruction.h"
#ifndef __M68000_BSET_H__
#define __M68000_BSET_H__
namespace M68000 {

class BSET :public M68000Instruction
{
public:
	virtual BSET* Clone() const {return new BSET();}
	virtual BSET* ClonePlacement(void* buffer) const {return new(buffer) BSET();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"0000***B11DDDDDD", L"B=1 DDDDDD=000000-000111,010000-110111,111000,111001");
		result &= table.AllocateRegionToOpcode(this, L"0000100B11DDDDDD", L"B=0 DDDDDD=000000-000111,010000-110111,111000,111001");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"BSET";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
		_size = (data.GetDataSegment(3, 3) == 0)? BITCOUNT_LONG: BITCOUNT_BYTE;
		if(data.GetBit(8))
		{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|---|---|-----------|-----------|
//	| 0 | 0 | 0 | 0 |  REGISTER | 1 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
			_source.BuildDataDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(_target.ExtensionSize());
			if((_target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect) || (_target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect))
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			}
		}
		else
		{
//	                                        |----------<ea>---------|
//	----------------------------------------=========================
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	|---|---|---|---|---|---|---|---|-------------------------------|
//	| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |       NUMBER OF THE BIT       |
//	-----------------------------------------------------------------

			_source.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(_source.ExtensionSize());
			_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(_target.ExtensionSize());
			if((_target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect) || (_target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect))
			{
				AddExecuteCycleCount(ExecuteTime(12, 2, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(12, 2, 1));
			}
		}
		AddExecuteCycleCount(_target.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Byte op1;
		Data op2(_size);
		Data result(_size);

		//Perform the operation
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		unsigned int _targetBit = (op1.GetData() % op2.GetBitCount());
		result = op2;
		result.SetBit(_targetBit, true);
		additionalTime += _target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetZ(!op2.GetBit(_targetBit));

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
	Bitcount _size;
	EffectiveAddress _source;
	EffectiveAddress _target;
};

} //Close namespace M68000
#endif
