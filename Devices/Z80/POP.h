#include "Z80Instruction.h"
#ifndef __Z80_POP_H__
#define __Z80_POP_H__
namespace Z80 {

class POP :public Z80Instruction
{
public:
	virtual POP* Clone() const {return new POP();}
	virtual POP* ClonePlacement(void* buffer) const {return new(buffer) POP();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"11**0001", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"POP";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _target.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_source.SetIndexState(GetIndexState(), GetIndexOffset());
		_target.SetIndexState(GetIndexState(), GetIndexOffset());

		// POP qq		11qq0001
		// POP IX		11011101 11100001
		// POP IY		11111101 11100001
		_source.SetMode(EffectiveAddress::Mode::SPPostInc);
		switch (data.GetDataSegment(4, 2))
		{
		case 0:
			_target.SetMode(EffectiveAddress::Mode::BC);
			break;
		case 1:
			_target.SetMode(EffectiveAddress::Mode::DE);
			break;
		case 2:
			_target.SetMode(EffectiveAddress::Mode::HL);
			break;
		case 3:
			_target.SetMode(EffectiveAddress::Mode::AF);
			break;
		}

		AddInstructionSize(GetIndexOffsetSize(_source.UsesIndexOffset() || _target.UsesIndexOffset()));
		AddInstructionSize(_source.ExtensionSize());
		AddInstructionSize(_target.ExtensionSize());
		AddExecuteCycleCount(10);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word result;

		// Perform the operation
		additionalTime += _source.Read(cpu, location, result);
		additionalTime += _target.Write(cpu, location, result);

		// Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
};

} // Close namespace Z80
#endif
