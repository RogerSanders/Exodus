#include "M68000Instruction.h"
#ifndef __M68000_RTS_H__
#define __M68000_RTS_H__
namespace M68000 {

class RTS :public M68000Instruction
{
public:
	virtual RTS* Clone() {return new RTS();}
	virtual RTS* ClonePlacement(void* buffer) {return new(buffer) RTS();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"0100111001110101", L"");
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"RTS", L"");
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
//	| 0 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 | 0 | 1 |
//	-----------------------------------------------------------------
		target.BuildAddressPostinc(BITCOUNT_LONG, location + GetInstructionSize(), M68000::SP);
		AddExecuteCycleCount(ExecuteTime(16, 4, 0));
	}	

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long address;

		//Perform the operation
		additionalTime += target.Read(cpu, address, GetInstructionRegister());
		cpu->PopCallStack(address.GetData());
		cpu->SetPC(address);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
