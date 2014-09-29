#include "M68000Instruction.h"
#ifndef __M68000_LEA_H__
#define __M68000_LEA_H__
namespace M68000 {

class LEA :public M68000Instruction
{
public:
	ExecuteTime GetExecuteTime(EffectiveAddress::Mode targetMode)
	{
		//##FIX## Note that while C++11 makes this thread safe, Visual Studio 2013 still
		//doesn't implement this feature. It's down on the conformance roadmap as
		//"Thread-safe function local static init", to be implemented in the near future
		//after VS2013.
		static const ExecuteTime executeTimeArray[9] = {
			//(An)                (An)+                -(An)                  d(An)                 d(An,ix)+              xxx.W                 xxx.L                  d(PC)                 d(PC,ix)
			ExecuteTime(4, 1, 0), ExecuteTime(0, 0, 0), ExecuteTime(0, 0, 0), ExecuteTime(8, 2, 0), ExecuteTime(12, 2, 0), ExecuteTime(8, 2, 0), ExecuteTime(12, 3, 0), ExecuteTime(8, 2, 0), ExecuteTime(12, 2, 0)};

		unsigned int targetIndex = 0;
		switch(targetMode)
		{
		case EffectiveAddress::Mode::AddRegIndirect:
			targetIndex = 0;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPostInc:
			targetIndex = 1;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPreDec:
			targetIndex = 2;
			break;
		case EffectiveAddress::Mode::AddRegIndirectDisplace:
			targetIndex = 3;
			break;
		case EffectiveAddress::Mode::AddRegIndirectIndex8Bit:
			targetIndex = 4;
			break;
		case EffectiveAddress::Mode::ABSWord:
			targetIndex = 5;
			break;
		case EffectiveAddress::Mode::ABSLong:
			targetIndex = 6;
			break;
		case EffectiveAddress::Mode::PCIndirectDisplace:
			targetIndex = 7;
			break;
		case EffectiveAddress::Mode::PCIndirectIndex8Bit:
			targetIndex = 8;
			break;
		}
		return executeTimeArray[targetIndex];
	}

	virtual LEA* Clone() const {return new LEA();}
	virtual LEA* ClonePlacement(void* buffer) const {return new(buffer) LEA();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"0100***111DDDDDD", L"DDDDDD=010000-010111,101000-110111,111000,111001,111010,111011");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LEA";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), source.Disassemble(labelSettings) + L", " + target.Disassemble(labelSettings));
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|---|---|-----------|-----------|
//	| 0 | 1 | 0 | 0 | REGISTER  | 1 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		source.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), BITCOUNT_LONG, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(source.ExtensionSize());
		target.BuildAddressDirect(BITCOUNT_LONG, location + GetInstructionSize(), data.GetDataSegment(9, 3));
		AddExecuteCycleCount(GetExecuteTime(source.GetAddressMode()));
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		M68000Long result;

		//If the target location was read from a memory address, and the read value hasn't
		//been modified up to this point, record the source memory address as a pointer to
		//code for disassembly purposes.
		unsigned int targetReadFromAddress;
		bool dataIsOffset;
		unsigned int offsetBaseAddress;
		unsigned int dataSize;
		bool dataUnmodified = target.IsTargetUnmodifiedFromMemoryReadV2(cpu, targetReadFromAddress, dataIsOffset, offsetBaseAddress, dataSize);
		//##FIX## Clean all this junk up
		dataUnmodified &= !dataIsOffset;

		//Perform the operation
		source.GetAddress(cpu, result);
		additionalTime += target.Write(cpu, result, GetInstructionRegister(), false, false, dataUnmodified, targetReadFromAddress, dataSize);

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
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace M68000
#endif
