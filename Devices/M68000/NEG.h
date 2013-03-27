#include "M68000Instruction.h"
#ifndef __M68000_NEG_H__
#define __M68000_NEG_H__
namespace M68000 {

class NEG :public M68000Instruction
{
public:
	virtual NEG* Clone() const {return new NEG();}
	virtual NEG* ClonePlacement(void* buffer) const {return new(buffer) NEG();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01000100CCDDDDDD", L"CC=00-10 DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"NEG";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(size), target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|-------|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 0 | 1 | 0 | 0 | SIZE  |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		switch(data.GetDataSegment(6, 2))
		{
		case 0:	//00
			size = BITCOUNT_BYTE;
			break;
		case 1:	//01
			size = BITCOUNT_WORD;
			break;
		case 2:	//10
			size = BITCOUNT_LONG;
			break;
		}

		//NEG	<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());

		if((target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT) || (target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT))
		{
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(6, 1, 0));
			}
		}
		else
		{
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(12, 1, 2));
			}
			AddExecuteCycleCount(target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size, 0);
		Data op2(size);
		Data result(size);

		//Perform the operation
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		result = op1 - op2;
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetX(op2.MSB() || result.MSB());
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(op2.MSB() && result.MSB());
		cpu->SetC(op2.MSB() || result.MSB());

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
