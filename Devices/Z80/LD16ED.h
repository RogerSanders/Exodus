#include "Z80Instruction.h"
#ifndef __Z80_LD16ED_H__
#define __Z80_LD16ED_H__
namespace Z80 {

class LD16ED :public Z80Instruction
{
public:
	virtual LD16ED* Clone() const {return new LD16ED();}
	virtual LD16ED* ClonePlacement(void* buffer) const {return new(buffer) LD16ED();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"01***011", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LD16ED";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(L"LD", _target.Disassemble() + L", " + _source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_source.SetIndexState(GetIndexState(), GetIndexOffset());
		_target.SetIndexState(GetIndexState(), GetIndexOffset());

		if(data.GetBit(3))
		{
			//LD dd,(nn)	11101101 01dd1011 nnnnnnnn nnnnnnnn
			_source.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
			_target.Decode16BitRegister(data.GetDataSegment(4, 2));
			AddExecuteCycleCount(16);
		}
		else
		{
			//LD (nn),dd	11101101 01dd0011 nnnnnnnn nnnnnnnn
			_source.Decode16BitRegister(data.GetDataSegment(4, 2));
			_target.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(16);
		}

		AddInstructionSize(_source.ExtensionSize());
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word result;

		//Perform the operation
		additionalTime += _source.Read(cpu, location, result);
		additionalTime += _target.Write(cpu, location, result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
};

} //Close namespace Z80
#endif
