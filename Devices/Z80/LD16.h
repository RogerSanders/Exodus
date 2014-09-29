#include "Z80Instruction.h"
#ifndef __Z80_LD16_H__
#define __Z80_LD16_H__
namespace Z80 {

class LD16 :public Z80Instruction
{
public:
	virtual LD16* Clone() const {return new LD16();}
	virtual LD16* ClonePlacement(void* buffer) const {return new(buffer) LD16();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"00**0001", L"");
		result &= table.AllocateRegionToOpcode(this, L"0010*010", L"");
		result &= table.AllocateRegionToOpcode(this, L"11111001", L"");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LD16";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(L"LD", target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		if(data == 0xF9)
		{
			//LD SP,HL		11111001
			//LD SP,IX		11011101 11111001
			//LD SP,IY		11111101 11111001
			source.SetMode(EffectiveAddress::Mode::HL);
			target.SetMode(EffectiveAddress::Mode::SP);
			AddExecuteCycleCount(6);
		}
		else if(data == 0x2A)
		{
			//LD HL,(nn)	00101010 nnnnnnnn nnnnnnnn
			//LD IX,(nn)	11011101 00101010 nnnnnnnn nnnnnnnn
			//LD IY,(nn)	11111101 00101010 nnnnnnnn nnnnnnnn
			source.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
			target.SetMode(EffectiveAddress::Mode::HL);
			AddExecuteCycleCount(16);
		}
		else if(data == 0x22)
		{
			//LD (nn),HL	00100010 nnnnnnnn nnnnnnnn
			//LD (nn),IX	11011101 00100010 nnnnnnnn nnnnnnnn
			//LD (nn),IY	11111101 00100010 nnnnnnnn nnnnnnnn
			source.SetMode(EffectiveAddress::Mode::HL);
			target.BuildAbsoluteAddress(location + GetInstructionSize(), cpu, transparent);
			AddExecuteCycleCount(16);
		}
		else
		{
			//LD dd,nn		00dd0001 nnnnnnnn nnnnnnnn
			//LD IX,nn		11011101 00100001 nnnnnnnn nnnnnnnn
			//LD IY,nn		11111101 00100001 nnnnnnnn nnnnnnnn
			source.BuildImmediateData(BITCOUNT_WORD, location + GetInstructionSize(), cpu, transparent);
			target.Decode16BitRegister(data.GetDataSegment(4, 2));
			AddExecuteCycleCount(10);
		}

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
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
