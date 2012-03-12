#include "M68000Instruction.h"
#ifndef __M68000_TAS_H__
#define __M68000_TAS_H__
namespace M68000 {

class TAS :public M68000Instruction
{
public:
	virtual TAS* Clone() {return new TAS();}
	virtual TAS* ClonePlacement(void* buffer) {return new(buffer) TAS();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"0100101011DDDDDD", L"DDDDDD=000000-000111,010000-110111,111000,111001");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"TAS", target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 1 | 0 | 1 | 0 | 1 | 1 |   MODE    |  REGISTER |
//	----------------------------------------=========================
//                                          |----------<ea>---------|
		//TAS	<ea>
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_BYTE, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());

		if((target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT) || (target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT))
		{
			AddExecuteCycleCount(ExecuteTime(4, 1, 0));
		}
		else
		{
			AddExecuteCycleCount(ExecuteTime(10, 1, 1));
			AddExecuteCycleCount(target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Byte op1;
		M68000Byte result;

		//##FIX## Reportedly, this instruction doesn't function correctly on the Mega
		//Drive. The following excerpt comes from Wikipedia:
		//"One of the 68000's instructions, TAS, is intended for semaphore communication
		//in multiprocessor machines and locks the 68000 bus during memory access. The
		//Sega hardware did not support this unusual bus cycle and ignored the write-back
		//phase. Two games, Gargoyles from Buena Vista Interactive, and Ex-Mutants from
		//Sega make use of the TAS instructions and expect it not to write to memory. As
		//a result, these games work on original Sega machines but not the Majesco
		//Genesis 3, which has correct support for TAS."
		//This indicates we might need some special-case support for this instruction.
		//After referring to the M68000 Users Manual, it is clear that the read component
		//will always succeed on any platform, since the Read-Modify-Write cycle starts
		//as a normal Read cycle, it just doesn't terminate the cycle before attempting
		//the write-back. It is this write-back that can fail, and does fail on the Mega
		//Drive. Support for this behaviour must not be implemented in the M68000 core,
		//in order to keep this core generic. The correct place to add, or rather remove
		//support for this bus cycle is in the bus itself. We should add a separate
		//function to the MemoryBus object called WriteBack, which is the second half of
		//this kind of indivisible operation. The first half will be implemented as a
		//normal read. We can add Lock() and Unlock() functions to guard bus access from
		//other devices during the chained Read()/WriteBack() calls.
		//##NOTE## 2011-09-03 Looking into this from a bus logic point of view, the only
		//difference between what happens in a TAS instruction and a separate read and
		//write, is that AS remains asserted for the duration. It is also worth noting
		//that it is the bus arbiter which asserts and negates DTACK, which is how the
		//M68K knows when a bus operation is complete. The DTACK is correctly asserted and
		//negated for both the read and write components of the TAS instruction, otherwise
		//the M68K bus would lock. Interestingly however, it is the VDP which actually
		//asserts the LWR and UWR lines, indicating a Lower Byte Write and Upper Byte
		//Write respectively. These lines are connected to the WE line inputs for the RAM
		//chips. Most likely, what is happening is that the VDP is calculating its output
		//CE line state based on the initial bus settings when AS is set, which at the
		//start of a TAS instruction indicates a read, so as a result, LWR and UWR are
		//negated. These lines most likely remain negated until AS is negated, and are not
		//re-evaluated as the bus state changes, so when the TAS instruction switches to
		//the write component, these lines are never asserted. As a result, the RAM chips
		//are never requested to write anything. We need to examine the datasheet for the
		//RAM chips to determine exactly what they would do if WE is not asserted; maybe
		//they perform another read instead of a write, or maybe they do nothing at all
		//and the bus arbiter controls DTACK anyway to complete the cycle. At any rate, we
		//can emulate this behaviour by adding a new pseudo CE line, which indicates
		//whether we're performing the second half of a TAS instruction, and emulate the
		//CE line state output of the VDP accordingly.

		//Perform the operation
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op1, GetInstructionRegister(), true, true);
		result = op1;
		result.SetBit(result.GetBitCount() - 1, true);
		additionalTime += target.Write(cpu, result, GetInstructionRegister(), true, false);

		//Set the flag results
		cpu->SetN(op1.Negative());
		cpu->SetZ(op1.Zero());
		cpu->SetV(false);
		cpu->SetC(false);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
