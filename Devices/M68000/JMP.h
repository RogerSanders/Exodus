#include "M68000Instruction.h"
#ifndef __M68000_JMP_H__
#define __M68000_JMP_H__
namespace M68000 {

class JMP :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode targetMode)
	{
		const ExecuteTime executeTimeArray[9] = {
			//(An)					(An)+					-(An)					d(An)					d(An,ix)+				xxx.W					xxx.L					d(PC)					d(PC,ix)
			ExecuteTime(8, 2, 0),	ExecuteTime(0, 0, 0),	ExecuteTime(0, 0, 0),	ExecuteTime(10, 2, 0),	ExecuteTime(14, 3, 0),	ExecuteTime(10, 2, 0),	ExecuteTime(12, 3, 0),	ExecuteTime(10, 2, 0),	ExecuteTime(14, 3, 0)};
		unsigned int targetIndex = 0;
		switch(targetMode)
		{
		case EffectiveAddress::ADDREG_INDIRECT:
			targetIndex = 0;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_POSTINC:
			targetIndex = 1;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_PREDEC:
			targetIndex = 2;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_DISPLACE:
			targetIndex = 3;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_INDEX_8BIT:
			targetIndex = 4;
			break;
		case EffectiveAddress::ABS_WORD:
			targetIndex = 5;
			break;
		case EffectiveAddress::ABS_LONG:
			targetIndex = 6;
			break;
		case EffectiveAddress::PC_INDIRECT_DISPLACE:
			targetIndex = 7;
			break;
		case EffectiveAddress::PC_INDIRECT_INDEX_8BIT:
			targetIndex = 8;
			break;
		}
		return executeTimeArray[targetIndex];
	}

	virtual JMP* Clone() {return new JMP();}
	virtual JMP* ClonePlacement(void* buffer) {return new(buffer) JMP();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"0100111011DDDDDD", L"DDDDDD=010000-010111,101000-110111,111000,111001,111010,111011");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"JMP", target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|

		//JMP	<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_LONG, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(GetExecuteTime(target.GetAddressMode()));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		M68000Long address;

		//If the target location was read from a memory address, and the read value hasn't
		//been modified up to this point, record the source memory address as a pointer to
		//code for disassembly purposes.
		unsigned int targetReadFromAddress;
		bool dataIsOffset;
		unsigned int offsetBaseAddress;
		unsigned int dataSize;
		if(target.IsTargetUnmodifiedFromMemoryReadV2(cpu, targetReadFromAddress, dataIsOffset, offsetBaseAddress, dataSize))
		{
			cpu->AddDisassemblyAddressInfoOffset(targetReadFromAddress, dataSize, true, dataIsOffset, offsetBaseAddress);
		}

		//Perform the operation
		target.GetAddress(cpu, address);
		cpu->SetPC(address);

		//Return the execution time
		return GetExecuteCycleCount();
	}

private:
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
