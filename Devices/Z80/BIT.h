#include "Z80Instruction.h"
#ifndef __Z80_BIT_H__
#define __Z80_BIT_H__
namespace Z80 {

class BIT :public Z80Instruction
{
public:
	virtual BIT* Clone() const {return new BIT();}
	virtual BIT* ClonePlacement(void* buffer) const {return new(buffer) BIT();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01******", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"BIT";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		doubleOutput = false;

		if(target.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//BIT b,r		11001011 01bbbrrr
			AddExecuteCycleCount(4);
		}
		else
		{
			//BIT b,(HL)		11001011 01bbb110
			//BIT b,(IX+d)		11011101 11001011 dddddddd 01bbb110
			//BIT b,(IY+d)		11111101 11001011 dddddddd 01bbb110
			target.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
			AddExecuteCycleCount(8);

			if(GetIndexState() != EffectiveAddress::INDEX_NONE)
			{
				doubleOutput = true;
				targetHL.SetIndexState(GetIndexState(), GetIndexOffset());
				targetHL.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
				AddExecuteCycleCount(4);
			}
		}
		source.BuildQuickData(data.GetDataSegment(3, 3));

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		bool result;

		//Perform the operation
		if(doubleOutput)
		{
			additionalTime += targetHL.Read(cpu, location, op1);
		}
		else
		{
			additionalTime += target.Read(cpu, location, op1);
		}
		Z80Byte bitNumber;
		additionalTime += source.Read(cpu, location, bitNumber);
		result = op1.GetBit(bitNumber.GetData());

		//Set the flag results
		//##NOTE## These flag calculations are right in the general case, but the results
		//for the undocumented flags Y and X are known to be incorrect in the case of the
		//"BIT n,(HL)", "BIT n,(IX+d)", and "BIT n,(IY+d)" forms. The full correct
		//behaviour is unknown at this time, and it looks like it will be a pain to
		//implement. See "The Undocumented Z80 Documented", section 4.1.
		cpu->SetFlagS((bitNumber == 7) && result);
		cpu->SetFlagZ(!result);
		cpu->SetFlagY((bitNumber == 5) && result);
		cpu->SetFlagH(true);
		cpu->SetFlagX((bitNumber == 3) && result);
		cpu->SetFlagPV(!result);
		cpu->SetFlagN(false);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	EffectiveAddress targetHL;
	bool doubleOutput;
};

} //Close namespace Z80
#endif
