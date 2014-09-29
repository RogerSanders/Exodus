#include "M68000Instruction.h"
#ifndef __M68000_NEGX_H__
#define __M68000_NEGX_H__
namespace M68000 {

class NEGX :public M68000Instruction
{
public:
	virtual NEGX* Clone() const {return new NEGX();}
	virtual NEGX* ClonePlacement(void* buffer) const {return new(buffer) NEGX();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01000000CCDDDDDD", L"CC=00-10 DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"NEGX";
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
//	| 0 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | SIZE  |    MODE   | REGISTER  |
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

		//NEGX	<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());

		if((target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect) || (target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect))
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
		Data op1(size);
		Data op2(size, 0);
		Data result(size);

		//Perform the operation
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op1, GetInstructionRegister());
		result = op2 - op1 - cpu->GetX();
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetX(op1.MSB() || result.MSB());
		cpu->SetN(result.Negative());
		cpu->SetZ(cpu->GetZ() && result.Zero());
		cpu->SetV(op1.MSB() && result.MSB());
		cpu->SetC(op1.MSB() || result.MSB());

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
