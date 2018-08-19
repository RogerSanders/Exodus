#include "M68000Instruction.h"
#ifndef __M68000_SUB_H__
#define __M68000_SUB_H__
namespace M68000 {

class SUB :public M68000Instruction
{
public:
	virtual SUB* Clone() const {return new SUB();}
	virtual SUB* ClonePlacement(void* buffer) const {return new(buffer) SUB();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1001***BCCDDDDDD", L"B=0 CC=00 DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"1001***BCCDDDDDD", L"B=0 CC=01-10 DDDDDD=000000-110111,111000,111001,111010,111011,111100");
		result &= table.AllocateRegionToOpcode(this, L"1001***BCCDDDDDD", L"B=1 CC=00-10 DDDDDD=010000-110111,111000,111001");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"SUB";
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
//	| 1 | 0 | 0 | 1 |  REGISTER | D | SIZE  |    MODE   |  REGISTER |
//	----------------------------------------=========================
//                              |--<opmode>-|----------<ea>---------|
		switch(data.GetDataSegment(6, 2))
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

		if(!data.GetBit(8))
		{
			//SUB	<ea>,Dn
			_target.BuildDataDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			_source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(_source.ExtensionSize());

			if(_size == BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(6, 1, 0));
				if((_source.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect) || (_source.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect) || (_source.GetAddressMode() == EffectiveAddress::Mode::Immediate))
				{
					AddExecuteCycleCount(ExecuteTime(2, 0, 0));
				}
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
			AddExecuteCycleCount(_source.DecodeTime());
		}
		else
		{
			//SUB	Dn,<ea>
			_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(_target.ExtensionSize());
			_source.BuildDataDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));

			if(_size == BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(12, 1, 2));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 1));
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

		//Perform the operation
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		result = op2 - op1;
		additionalTime += _target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		bool overflow = (op1.MSB() == result.MSB()) && (op2.MSB() != op1.MSB());
		bool borrow = (op1.MSB() && result.MSB()) || (!op2.MSB() && (op1.MSB() || result.MSB()));
		cpu->SetX(borrow);
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(overflow);
		cpu->SetC(borrow);

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
