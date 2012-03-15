#include "Z80Instruction.h"
#ifndef __Z80_EXX_H__
#define __Z80_EXX_H__
namespace Z80 {

class EXX :public Z80Instruction
{
public:
	virtual EXX* Clone() {return new EXX();}
	virtual EXX* ClonePlacement(void* buffer) {return new(buffer) EXX();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable& table)
	{
		return table.AllocateRegionToOpcode(this, L"11011001", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"EXX", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		//##NOTE## This opcode is not affected by index prefixes. HL is always the target.
		//EXX		11011001
		sourceBC.SetMode(EffectiveAddress::MODE_BC);
		sourceDE.SetMode(EffectiveAddress::MODE_DE);
		sourceHL.SetMode(EffectiveAddress::MODE_HL);
		targetBC.SetMode(EffectiveAddress::MODE_BC2);
		targetDE.SetMode(EffectiveAddress::MODE_DE2);
		targetHL.SetMode(EffectiveAddress::MODE_HL2);
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word sourceBCTemp;
		Z80Word sourceDETemp;
		Z80Word sourceHLTemp;
		Z80Word targetBCTemp;
		Z80Word targetDETemp;
		Z80Word targetHLTemp;

		//Perform the operation
		additionalTime += sourceBC.Read(cpu, location + GetInstructionSize(), sourceBCTemp);
		additionalTime += sourceDE.Read(cpu, location + GetInstructionSize(), sourceDETemp);
		additionalTime += sourceHL.Read(cpu, location + GetInstructionSize(), sourceHLTemp);
		additionalTime += targetBC.Read(cpu, location + GetInstructionSize(), targetBCTemp);
		additionalTime += targetDE.Read(cpu, location + GetInstructionSize(), targetDETemp);
		additionalTime += targetHL.Read(cpu, location + GetInstructionSize(), targetHLTemp);
		additionalTime += sourceBC.Write(cpu, location + GetInstructionSize(), targetBCTemp);
		additionalTime += sourceDE.Write(cpu, location + GetInstructionSize(), targetDETemp);
		additionalTime += sourceHL.Write(cpu, location + GetInstructionSize(), targetHLTemp);
		additionalTime += targetBC.Write(cpu, location + GetInstructionSize(), sourceBCTemp);
		additionalTime += targetDE.Write(cpu, location + GetInstructionSize(), sourceDETemp);
		additionalTime += targetHL.Write(cpu, location + GetInstructionSize(), sourceHLTemp);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress sourceBC;
	EffectiveAddress sourceDE;
	EffectiveAddress sourceHL;
	EffectiveAddress targetBC;
	EffectiveAddress targetDE;
	EffectiveAddress targetHL;
};

} //Close namespace Z80
#endif
