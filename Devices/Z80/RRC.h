#include "Z80Instruction.h"
#ifndef __Z80_RRC_H__
#define __Z80_RRC_H__
namespace Z80 {

class RRC :public Z80Instruction
{
public:
	virtual RRC* Clone() const {return new RRC();}
	virtual RRC* ClonePlacement(void* buffer) const {return new(buffer) RRC();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00001***", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RRC";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _target.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_target.SetIndexState(GetIndexState(), GetIndexOffset());
		_doubleOutput = false;

		if (_target.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			// RRC r			11001011 00001rrr
			AddExecuteCycleCount(4);
		}
		else
		{
			// RRC (HL)		11001011 00001110
			// RRC (IX+d)	11011101 11001011 dddddddd 00001110
			// RRC (IY+d)	11111101 11001011 dddddddd 00001110
			_target.SetMode(EffectiveAddress::Mode::HLIndirect);
			AddExecuteCycleCount(11);

			if (GetIndexState() != EffectiveAddress::IndexState::None)
			{
				_doubleOutput = true;
				_targetHL.SetIndexState(GetIndexState(), GetIndexOffset());
				_targetHL.SetMode(EffectiveAddress::Mode::HLIndirect);
				AddExecuteCycleCount(4);
			}
		}

		AddInstructionSize(GetIndexOffsetSize(_target.UsesIndexOffset()));
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		// Perform the operation
		if (_doubleOutput)
		{
			additionalTime += _targetHL.Read(cpu, location, op1);
		}
		else
		{
			additionalTime += _target.Read(cpu, location, op1);
		}
		result = (op1 >> 1);
		result.SetBit(result.GetBitCount() - 1, op1.GetBit(0));
		if (_doubleOutput)
		{
			additionalTime += _targetHL.Write(cpu, location, result);
		}
		additionalTime += _target.Write(cpu, location, result);

		// Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV(result.ParityEven());
		cpu->SetFlagN(false);
		cpu->SetFlagC(op1.GetBit(0));

		// Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _target;
	EffectiveAddress _targetHL;
	bool _doubleOutput;
};

} // Close namespace Z80
#endif
