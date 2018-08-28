#include "Z80Instruction.h"
#ifndef __Z80_RES_H__
#define __Z80_RES_H__
namespace Z80 {

class RES :public Z80Instruction
{
public:
	virtual RES* Clone() const {return new RES();}
	virtual RES* ClonePlacement(void* buffer) const {return new(buffer) RES();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"10******", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RES";
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
			// RES b,r		11001011 10bbbrrr
			AddExecuteCycleCount(4);
		}
		else
		{
			// RES b,(HL)		11001011 10bbb110
			// RES b,(IX+d)		11011101 11001011 dddddddd 10bbb110
			// RES b,(IY+d)		11111101 11001011 dddddddd 10bbb110
			_target.SetMode(EffectiveAddress::Mode::HLIndirect);
			AddExecuteCycleCount(7);

			if (GetIndexState() != EffectiveAddress::IndexState::None)
			{
				_doubleOutput = true;
				_targetHL.SetIndexState(GetIndexState(), GetIndexOffset());
				_targetHL.SetMode(EffectiveAddress::Mode::HLIndirect);
				AddExecuteCycleCount(8);
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
		op1.SetBit(bitNumber.GetData(), false);
		if (_doubleOutput)
		{
			additionalTime += _targetHL.Write(cpu, location, op1);
		}
		additionalTime += _target.Write(cpu, location, op1);

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
