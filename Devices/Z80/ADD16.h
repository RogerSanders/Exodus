#include "Z80Instruction.h"
#ifndef __Z80_ADD16_H__
#define __Z80_ADD16_H__
namespace Z80 {

class ADD16 :public Z80Instruction
{
public:
	virtual ADD16* Clone() const {return new ADD16();}
	virtual ADD16* ClonePlacement(void* buffer) const {return new(buffer) ADD16();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00**1001", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ADD16";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(L"ADD", target.Disassemble() + L", " + source.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetMode(EffectiveAddress::MODE_HL);

		//ADD HL,ss		00ss1001
		//ADD IX,ss		11011101 00ss1001
		//ADD IY,ss		11111101 00ss1001
		source.Decode16BitRegister(data.GetDataSegment(4, 2));

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(11);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word op1;
		Z80Word op2;
		Z80Word result;

		//Perform the operation
		additionalTime += source.Read(cpu, location, op1);
		additionalTime += target.Read(cpu, location, op2);
		result = op2 + op1;
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagY(result.GetBit(8+5));
		cpu->SetFlagH((op1.GetDataSegment(0, 8+4) + op2.GetDataSegment(0, 8+4)) > result.GetDataSegment(0, 8+4));
		cpu->SetFlagX(result.GetBit(8+3));
		cpu->SetFlagN(false);
		cpu->SetFlagC((op1.GetData() + op2.GetData()) > result.GetData());

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
