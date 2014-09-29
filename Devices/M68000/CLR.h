#include "M68000Instruction.h"
#ifndef __M68000_CLR_H__
#define __M68000_CLR_H__
namespace M68000 {

class CLR :public M68000Instruction
{
public:
	virtual CLR* Clone() const {return new CLR();}
	virtual CLR* ClonePlacement(void* buffer) const {return new(buffer) CLR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01000010CCDDDDDD", L"CC=00-10 DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"CLR";
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
//	| 0 | 1 | 0 | 0 | 0 | 0 | 1 | 0 |  SIZE |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                                  <ea>
		switch(data.GetDataSegment(6, 2))
		{
		case 0: //00
			size = BITCOUNT_BYTE;
			break;
		case 1:	//01
			size = BITCOUNT_WORD;
			break;
		case 2:	//10
			size = BITCOUNT_LONG;
			break;
		}

		//CLR	<ea>
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
		Data result(size);

		//Perform the operation
		//##NOTE## As per the official documentation, we read the target address before
		//clearing it. This can be important if the target is a device rather than just a
		//storage location. See the	M68000 Programmer's Reference Manual, section 4, page
		//74 for more info.
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, result, GetInstructionRegister());
		result = 0;
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetN(false);
		cpu->SetZ(true);
		cpu->SetV(false);
		cpu->SetC(false);

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
