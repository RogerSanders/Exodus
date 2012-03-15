#include "Z80Instruction.h"
#ifndef __Z80_IM_H__
#define __Z80_IM_H__
namespace Z80 {

class IM :public Z80Instruction
{
public:
	virtual IM* Clone() {return new IM();}
	virtual IM* ClonePlacement(void* buffer) {return new(buffer) IM();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"01000110", L"");
		result &= table.AllocateRegionToOpcode(this, L"01010110", L"");
		result &= table.AllocateRegionToOpcode(this, L"01011110", L"");
		return result;
	}

	virtual Disassembly Z80Disassemble()
	{
		std::wstringstream argumentDisassembly;
		argumentDisassembly << newInterruptMode;
		return Disassembly(L"IM", argumentDisassembly.str());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		//##TODO## Determine which mode the undocumented "IM 0/1*" modes actually set
		switch(data.GetDataSegment(3, 2))
		{
		case 0:
			newInterruptMode = 0;
			break;
		case 2:
			newInterruptMode = 1;
			break;
		case 3:
			newInterruptMode = 2;
			break;
		}
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		//Perform the operation
		cpu->SetInterruptMode(newInterruptMode);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount();
	}

private:
	unsigned int newInterruptMode;
};

} //Close namespace Z80
#endif
