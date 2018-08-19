#include "Z80Instruction.h"
#ifndef __Z80_RST_H__
#define __Z80_RST_H__
namespace Z80 {

class RST :public Z80Instruction
{
public:
	virtual RST* Clone() const {return new RST();}
	virtual RST* ClonePlacement(void* buffer) const {return new(buffer) RST();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"11***111", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"RST";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_target.SetIndexState(GetIndexState(), GetIndexOffset());

		// RST p		11ttt111
		_target.SetMode(EffectiveAddress::Mode::SPPreDec);
		_source.BuildImmediateData(Z80Word(data.GetDataSegment(3, 3) << 3));
		AddExecuteCycleCount(11);

		AddInstructionSize(GetIndexOffsetSize(_target.UsesIndexOffset()));
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word newPC;

		// Perform the operation
		additionalTime += _source.Read(cpu, location + GetInstructionSize(), newPC);
		additionalTime += _target.Write(cpu, location + GetInstructionSize(), location + GetInstructionSize());
		cpu->PushCallStack(cpu->GetPC().GetData(), newPC.GetData(), (location + GetInstructionSize()).GetData(), L"RST");
		cpu->SetPC(newPC);

		// Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
};

} // Close namespace Z80
#endif
