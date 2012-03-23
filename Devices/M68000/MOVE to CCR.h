#include "M68000Instruction.h"
#ifndef __M68000_MOVE_TO_CCR_H__
#define __M68000_MOVE_TO_CCR_H__
namespace M68000 {

class MOVE_to_CCR :public M68000Instruction
{
public:
	virtual MOVE_to_CCR* Clone() const {return new MOVE_to_CCR();}
	virtual MOVE_to_CCR* ClonePlacement(void* buffer) const {return new(buffer) MOVE_to_CCR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100010011DDDDDD", L"DDDDDD=000000-000111,010000-110111,111000,111001,111010,111011,111100");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MOVE to CCR";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(L"MOVE", source.Disassemble() + L", CCR");
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 0 | 1 | 0 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|

		//MOVE	<ea>,CCR
		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		AddExecuteCycleCount(ExecuteTime(12, 1, 0));
		AddExecuteCycleCount(source.DecodeTime());
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Word result;

		//Perform the operation
		additionalTime += source.Read(cpu, result, GetInstructionRegister());
		cpu->SetCCR(M68000Byte(result.Convert(BITCOUNT_BYTE)));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
};

} //Close namespace M68000
#endif
