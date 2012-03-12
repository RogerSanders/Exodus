#include "M68000Instruction.h"
#ifndef __M68000_MOVE_USP_H__
#define __M68000_MOVE_USP_H__
namespace M68000 {

class MOVE_USP :public M68000Instruction
{
public:
	virtual MOVE_USP* Clone() {return new MOVE_USP();}
	virtual MOVE_USP* ClonePlacement(void* buffer) {return new(buffer) MOVE_USP();}

	virtual bool Privileged() const
	{
		return true;
	}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"010011100110****", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		std::wstring argumentDisassembly;

		if(dr == 0)
		{
			//MOVE	An,USP
			argumentDisassembly = source.Disassemble() + L", USP";
		}
		else
		{
			//MOVE	USP,An
			argumentDisassembly = L"USP, " + source.Disassemble();
		}

		return Disassembly(L"MOVE", argumentDisassembly);
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 0 |dr | REGISTER  |
//	-----------------------------------------------------------------

//##TODO## Consider splitting this opcode into "Move to USP" and "Move from USP"
		dr = data.GetDataSegment(3, 1);
		source.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(0, 3));
		AddExecuteCycleCount(ExecuteTime(4, 1, 0));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long result;

		//Perform the operation
		if(dr == 0)
		{
		//MOVE	An,USP
			additionalTime += source.Read(cpu, result, GetInstructionRegister());
			cpu->SetUSP(result);
		}
		else
		{
		//MOVE	USP,An
			cpu->GetUSP(result);
			additionalTime += source.Write(cpu, result, GetInstructionRegister());
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	unsigned int dr;
	EffectiveAddress source;
};

} //Close namespace M68000
#endif