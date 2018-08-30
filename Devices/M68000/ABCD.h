#include "M68000Instruction.h"
#ifndef __M68000_ABCD_H__
#define __M68000_ABCD_H__
namespace M68000 {

class ABCD :public M68000Instruction
{
public:
	virtual ABCD* Clone() const {return new ABCD();}
	virtual ABCD* ClonePlacement(void* buffer) const {return new(buffer) ABCD();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"1100***10000****", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ABCD";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|---|---|---|---|---|-----------|
//	| 1 | 1 | 0 | 0 |    Rx     | 1 | 0 | 0 | 0 | 0 |R/M|    Ry     |
//	-----------------------------------------------------------------
		if (!data.GetBit(3))
		{
			_source.BuildDataDirect(BITCOUNT_BYTE, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			_target.BuildDataDirect(BITCOUNT_BYTE, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			AddExecuteCycleCount(ExecuteTime(6, 1, 0));
		}
		else
		{
			_source.BuildAddressPredec(BITCOUNT_BYTE, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			_target.BuildAddressPredec(BITCOUNT_BYTE, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			AddExecuteCycleCount(ExecuteTime(18, 3, 1));
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Byte op1Base10;
		M68000Byte op2Base10;

		// Perform the operation
		additionalTime += _source.Read(cpu, op1Base10, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2Base10, GetInstructionRegister());

		// This code is based on information provided by flamewing from the following source:
		// http://gendev.spritesmind.net/forum/viewtopic.php?p=30943#p30943
		// Correct behaviour of the result is tested and confirmed by "bcd-verifier-u1.bin"
		M68000Byte result = op2Base10 + op1Base10 + cpu->GetX();
		M68000Byte binaryCarry = ((op2Base10 & op1Base10) | (~result & op2Base10) | (~result & op1Base10)) & 0x88;
		M68000Byte decimalCarry = M68000Byte(((M68000Word(result) + 0x66) ^ M68000Word(result)) >> 1) & 0x88;
		M68000Byte correctionFactor = (binaryCarry | decimalCarry) - ((binaryCarry | decimalCarry) >> 2);
		M68000Byte resultBase10 = result + correctionFactor;
		additionalTime += _target.Write(cpu, resultBase10, GetInstructionRegister());

		// Set the flag results
		bool carry = (binaryCarry | (result & ~resultBase10)).MSB();
		cpu->SetX(carry);
		cpu->SetZ(cpu->GetZ() && resultBase10.Zero());
		cpu->SetC(carry);
		cpu->SetN(resultBase10.Negative());
		cpu->SetV((~result & resultBase10).MSB());

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
};

} // Close namespace M68000
#endif
