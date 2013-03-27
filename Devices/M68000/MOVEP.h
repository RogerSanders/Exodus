#include "M68000Instruction.h"
#ifndef __M68000_MOVEP_H__
#define __M68000_MOVEP_H__
namespace M68000 {

class MOVEP :public M68000Instruction
{
public:
	virtual MOVEP* Clone() const {return new MOVEP();}
	virtual MOVEP* ClonePlacement(void* buffer) const {return new(buffer) MOVEP();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0000***1**001***", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MOVEP";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(size), source.Disassemble(labelSettings) + L", " + target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	                            |---opmode--|
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|-----------|---|---|---|-----------|
//	| 0 | 0 | 0 | 0 |Dx REGISTER| 1 | D | S | 0 | 0 | 1 |Ay REGISTER|
//	|---------------------------------------------------------------|
//	|                        16 BITS OFFSET                         |
//	-----------------------------------------------------------------
		if(!data.GetBit(6))
		{
			size = BITCOUNT_WORD;
		}
		else
		{
			size = BITCOUNT_LONG;
		}

		memoryTarget = data.GetBit(7);

		if(memoryTarget)
		{
			//MOVEP	Dx,(d,Ay)
			source.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			target.BuildAddressIndirectDisplace(size, location + GetInstructionSize(), data.GetDataSegment(0, 3), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(target.ExtensionSize());
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(16, 4, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(24, 6, 0));
			}
		}
		else
		{
			//MOVEP	(d,Ay),Dx
			source.BuildAddressIndirectDisplace(size, location + GetInstructionSize(), data.GetDataSegment(0, 3), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(source.ExtensionSize());
			target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(16, 2, 2));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(24, 2, 4));
			}
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		EffectiveAddress newTarget(target);
		EffectiveAddress newSource(source);

		if(memoryTarget)
		{
			//Move the data from the source to the target memory location
			Data totalData(size);
			additionalTime += source.Read(cpu, totalData, GetInstructionRegister());
			M68000Long address;
			target.GetAddress(cpu, address);
			for(unsigned int i = 0; i < totalData.GetByteSize(); ++i)
			{
				M68000Byte byte = totalData.GetByte((totalData.GetByteSize() - 1) - i);
				additionalTime += cpu->WriteMemory(address, byte, cpu->GetFunctionCode(false), location + 2, true, GetInstructionRegister(), false, false);
				address += 2;
			}
		}
		else
		{
			//Read the data from the memory address and load it into the target
			M68000Long address;
			source.GetAddress(cpu, address);
			Data totalData(size);
			totalData = 0;
			for(unsigned int i = 0; i < totalData.GetByteSize(); ++i)
			{
				M68000Byte byte;

				//Record active disassembly info for this register move
				cpu->AddDisassemblyAddressInfoData(address.GetData(), byte.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER);

				additionalTime += cpu->ReadMemory(address, byte, cpu->GetFunctionCode(false), location + 2, true, GetInstructionRegister(), false, false);
				address += 2;
				totalData <<= byte.GetBitCount();
				totalData.SetLowerBits(byte);
			}
			additionalTime += target.Write(cpu, totalData, GetInstructionRegister());
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		source.AddLabelTargetsToSet(labelTargetLocations);
		target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	bool memoryTarget;
	Bitcount size;
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
