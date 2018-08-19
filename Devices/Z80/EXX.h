#include "Z80Instruction.h"
#ifndef __Z80_EXX_H__
#define __Z80_EXX_H__
namespace Z80 {

class EXX :public Z80Instruction
{
public:
	virtual EXX* Clone() const {return new EXX();}
	virtual EXX* ClonePlacement(void* buffer) const {return new(buffer) EXX();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"11011001", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"EXX";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), L"");
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		//##NOTE## This opcode is not affected by index prefixes. HL is always the _target.
		//EXX		11011001
		_sourceBC.SetMode(EffectiveAddress::Mode::BC);
		_sourceDE.SetMode(EffectiveAddress::Mode::DE);
		_sourceHL.SetMode(EffectiveAddress::Mode::HL);
		_targetBC.SetMode(EffectiveAddress::Mode::BC2);
		_targetDE.SetMode(EffectiveAddress::Mode::DE2);
		_targetHL.SetMode(EffectiveAddress::Mode::HL2);
		AddExecuteCycleCount(4);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word _sourceBCTemp;
		Z80Word _sourceDETemp;
		Z80Word _sourceHLTemp;
		Z80Word _targetBCTemp;
		Z80Word _targetDETemp;
		Z80Word _targetHLTemp;

		//Perform the operation
		additionalTime += _sourceBC.Read(cpu, location + GetInstructionSize(), _sourceBCTemp);
		additionalTime += _sourceDE.Read(cpu, location + GetInstructionSize(), _sourceDETemp);
		additionalTime += _sourceHL.Read(cpu, location + GetInstructionSize(), _sourceHLTemp);
		additionalTime += _targetBC.Read(cpu, location + GetInstructionSize(), _targetBCTemp);
		additionalTime += _targetDE.Read(cpu, location + GetInstructionSize(), _targetDETemp);
		additionalTime += _targetHL.Read(cpu, location + GetInstructionSize(), _targetHLTemp);
		additionalTime += _sourceBC.Write(cpu, location + GetInstructionSize(), _targetBCTemp);
		additionalTime += _sourceDE.Write(cpu, location + GetInstructionSize(), _targetDETemp);
		additionalTime += _sourceHL.Write(cpu, location + GetInstructionSize(), _targetHLTemp);
		additionalTime += _targetBC.Write(cpu, location + GetInstructionSize(), _sourceBCTemp);
		additionalTime += _targetDE.Write(cpu, location + GetInstructionSize(), _sourceDETemp);
		additionalTime += _targetHL.Write(cpu, location + GetInstructionSize(), _sourceHLTemp);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _sourceBC;
	EffectiveAddress _sourceDE;
	EffectiveAddress _sourceHL;
	EffectiveAddress _targetBC;
	EffectiveAddress _targetDE;
	EffectiveAddress _targetHL;
};

} //Close namespace Z80
#endif
