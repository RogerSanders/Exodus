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
		return Disassembly(GetOpcodeName(), _target.Disassemble() + L", " + _source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_target.SetIndexState(GetIndexState(), GetIndexOffset());
		_doubleOutput = false;

		if (_target.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			// BIT b,r		11001011 01bbbrrr
			AddExecuteCycleCount(4);
		}
		else
		{
			// BIT b,(HL)		11001011 01bbb110
			// BIT b,(IX+d)		11011101 11001011 dddddddd 01bbb110
			// BIT b,(IY+d)		11111101 11001011 dddddddd 01bbb110
			_target.SetMode(EffectiveAddress::Mode::HLIndirect);
			AddExecuteCycleCount(8);

			if (GetIndexState() != EffectiveAddress::IndexState::None)
			{
				_doubleOutput = true;
				_targetHL.SetIndexState(GetIndexState(), GetIndexOffset());
				_targetHL.SetMode(EffectiveAddress::Mode::HLIndirect);
				AddExecuteCycleCount(4);
			}
		}
		_source.BuildQuickData(data.GetDataSegment(3, 3));

		AddInstructionSize(GetIndexOffsetSize(_target.UsesIndexOffset()));
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		bool result;

		// Perform the operation
		if (_doubleOutput)
		{
			additionalTime += _targetHL.Read(cpu, location, op1);
		}
		else
		{
			additionalTime += _target.Read(cpu, location, op1);
		}
		Z80Byte bitNumber;
		additionalTime += _source.Read(cpu, location, bitNumber);
		result = op1.GetBit(bitNumber.GetData());

		// Set the flag results
		//##NOTE## These flag calculations are right in the general case, but the results
		// for the undocumented flags Y and X are known to be incorrect in the case of the
		//"BIT n,(HL)", "BIT n,(IX+d)", and "BIT n,(IY+d)" forms. The full correct
		// behaviour is unknown at this time, and it looks like it will be a pain to
		// implement. See "The Undocumented Z80 Documented", section 4.1.
		cpu->SetFlagS((bitNumber == 7) && result);
		cpu->SetFlagZ(!result);
		cpu->SetFlagY((bitNumber == 5) && result);
		cpu->SetFlagH(true);
		cpu->SetFlagX((bitNumber == 3) && result);
		cpu->SetFlagPV(!result);
		cpu->SetFlagN(false);

		// Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
	EffectiveAddress _targetHL;
	bool _doubleOutput;
};

} // Close namespace Z80
#endif
