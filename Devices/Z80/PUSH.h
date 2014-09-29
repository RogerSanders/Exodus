#include "Z80Instruction.h"
#ifndef __Z80_PUSH_H__
#define __Z80_PUSH_H__
namespace Z80 {

class PUSH :public Z80Instruction
{
public:
	virtual PUSH* Clone() const {return new PUSH();}
	virtual PUSH* ClonePlacement(void* buffer) const {return new(buffer) PUSH();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"11**0101", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"PUSH";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		//PUSH qq		11qq0101
		//PUSH IX		11011101 11100101
		//PUSH IY		11111101 11100101
		switch(data.GetDataSegment(4, 2))
		{
		case 0:
			source.SetMode(EffectiveAddress::Mode::BC);
			break;
		case 1:
			source.SetMode(EffectiveAddress::Mode::DE);
			break;
		case 2:
			source.SetMode(EffectiveAddress::Mode::HL);
			break;
		case 3:
			source.SetMode(EffectiveAddress::Mode::AF);
			break;
		}
		target.SetMode(EffectiveAddress::Mode::SPPreDec);

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(11);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word result;

		//Perform the operation
		additionalTime += source.Read(cpu, location, result);
		additionalTime += target.Write(cpu, location, result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
