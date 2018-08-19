#include "M68000Instruction.h"
#ifndef __M68000_ADDQ_H__
#define __M68000_ADDQ_H__
namespace M68000 {

class ADDQ :public M68000Instruction
{
public:
	virtual ADDQ* Clone() const {return new ADDQ();}
	virtual ADDQ* ClonePlacement(void* buffer) const {return new(buffer) ADDQ();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"0101***0CCDDDDDD", L"CC=00 DDDDDD=000000-000111,010000-110111,111000,111001");
		result &= table.AllocateRegionToOpcode(this, L"0101***0CCDDDDDD", L"CC=01-10 DDDDDD=000000-110111,111000,111001");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ADDQ";
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
//	| 0 | 1 | 0 | 1 |    DATA   | 0 | SIZE  |    MODE   |  REGISTER |
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

		//ADDQ	#<data>,<ea>
		_source.BuildQuickData(location + GetInstructionSize(), data.GetDataSegment(9, 3));
		_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(_target.ExtensionSize());

		if(_target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect)
		{
			if(_size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 0));
			}
		}
		else if(_target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect)
		{
			AddExecuteCycleCount(ExecuteTime(8, 1, 0));
		}
		else
		{
			if(_size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(12, 1, 2));
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

		//Change the operation size to long if the _target is an address register. See the
		//description of the opcode in the 68000 Programmer's Manual for more info on
		//this behaviour. We do this test here instead of the decode function so that the
		//disassembly correctly shows the specified size of the operation, even if it has
		//no meaning on an address register.
		if(_target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect)
		{
			op1.Resize(BITCOUNT_LONG);
			op2.Resize(BITCOUNT_LONG);
			result.Resize(BITCOUNT_LONG);
		}

		//Perform the operation
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		result = op2 + op1;
		additionalTime += _target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		if(_target.GetAddressMode() != EffectiveAddress::Mode::AddRegDirect)
		{
			bool carry = (op1.MSB() && op2.MSB()) || (!result.MSB() && (op1.MSB() || op2.MSB())) ;
			bool overflow = (op1.MSB() == op2.MSB()) && (result.MSB() != (op1.MSB() && op2.MSB()));
			cpu->SetX(carry);
			cpu->SetN(result.Negative());
			cpu->SetZ(result.Zero());
			cpu->SetV(overflow);
			cpu->SetC(carry);
		}

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
