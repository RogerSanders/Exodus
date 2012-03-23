#include "M68000Instruction.h"
#ifndef __M68000_Scc_H__
#define __M68000_Scc_H__
namespace M68000 {

class Scc :public M68000Instruction
{
public:
	virtual Scc* Clone() const {return new Scc();}
	virtual Scc* ClonePlacement(void* buffer) const {return new(buffer) Scc();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0101****11DDDDDD", L"DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"Scc";
	}

	virtual Disassembly M68000Disassemble() const
	{
		return Disassembly(L"S" + DisassembleConditionCode(conditionCode), target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---------------|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 1 | cc CONDITION  | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		conditionCode = (ConditionCode)data.GetDataSegment(8, 4);

		//Scc	<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());

		if((target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT) || (target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT))
		{
			AddExecuteCycleCount(ExecuteTime(4, 1, 0));
		}
		else
		{
			AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			AddExecuteCycleCount(target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		bool result = ConditionCodeTrue(cpu, conditionCode);

		//Perform the operation
		M68000Byte temp;
		temp = (result)? 0xFF: 0x00;
		additionalTime += target.Write(cpu, temp, GetInstructionRegister());

		//Calculate the additional execution time
		ExecuteTime additionalCycles;
		if(result && ((target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT) || (target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT)))
		{
			additionalCycles.Set(2, 0, 0);
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

private:
	ConditionCode conditionCode;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
