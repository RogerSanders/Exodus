#include "Z80Instruction.h"
#ifndef __Z80_DJNZ_H__
#define __Z80_DJNZ_H__
namespace Z80 {

class DJNZ :public Z80Instruction
{
public:
	virtual DJNZ* Clone() const {return new DJNZ();}
	virtual DJNZ* ClonePlacement(void* buffer) const {return new(buffer) DJNZ();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00010000", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"DJNZ";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), target.Disassemble(), target.DisassembleImmediateAsPCDisplacement(GetInstructionLocation() + GetInstructionSize()));
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		//DJNZ e		00010000 eeeeeeee
		source.SetMode(EffectiveAddress::Mode::B);
		target.BuildImmediateData(BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent);
		AddExecuteCycleCount(8);

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte count;
		Z80Byte offset;
		Z80Word newPC;
		ExecuteTime additionalCycles;

		//Update the counter
		additionalTime += source.Read(cpu, location, count);
		--count;
		additionalTime += source.Write(cpu, location, count);

		if(count != 0)
		{
			//If the counter is nonzero, run the loop again
			additionalTime += target.Read(cpu, location, offset);
			newPC = location + GetInstructionSize();
			newPC += Z80Word(offset.SignExtend(BITCOUNT_WORD));
			additionalCycles.cycles = 5;
		}
		else
		{
			//The counter has reached zero. Continue execution at the next instruction.
			newPC = location + GetInstructionSize();
		}
		cpu->SetPC(newPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
