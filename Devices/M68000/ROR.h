#include "M68000Instruction.h"
#ifndef __M68000_ROR_H__
#define __M68000_ROR_H__
namespace M68000 {

class ROR :public M68000Instruction
{
public:
	virtual ROR* Clone() const {return new ROR();}
	virtual ROR* ClonePlacement(void* buffer) const {return new(buffer) ROR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1110***0CC*11***", L"CC=00-10");
		result &= table.AllocateRegionToOpcode(this, L"1110011011DDDDDD", L"DDDDDD=010000-110111,111000,111001");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ROR";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		if(target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT)
		{
			return Disassembly(GetOpcodeName() + L"." + DisassembleSize(size), source.Disassemble(labelSettings) + L", " + target.Disassemble(labelSettings));
		}
		else
		{
			return Disassembly(GetOpcodeName(), target.Disassemble(labelSettings));
		}
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	In the case of the rotating of a register:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|---|---|---|-----------|
//	| 1 | 1 | 1 | 0 |  NUMBER/  | 0 |  SIZE |i/r| 1 | 1 | REGISTER  |
//	|   |   |   |   |  REGISTER |   |       |   |   |   |           |
//	-----------------------------------------------------------------
		if(data.GetDataSegment(6, 2) != 3)
		{
			switch(data.GetDataSegment(6, 2))
			{
			case 0:	//00
				size = BITCOUNT_BYTE;
				break;
			case 1:	//01
				size = BITCOUNT_WORD;
				break;
			case 2:	//10
				size = BITCOUNT_LONG;
				break;
			}

			target.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			if(!data.GetBit(5))
			{
				source.BuildQuickData(location + GetInstructionSize(), data.GetDataSegment(9, 3));
			}
			else
			{
				source.BuildDataDirect(size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			}

			if(size == BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(6, 1, 0));
			}
		}
		else
//	In the case of the rotating of a memory area:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 1 | 1 | 1 | 0 | 0 | 1 | 1 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		{
			size = BITCOUNT_WORD;
			target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(target.ExtensionSize());
			source.BuildImmediateData(location + GetInstructionSize(), M68000Word(1));

			AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			AddExecuteCycleCount(target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size);
		Data op2(size);
		Data result(size);

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		op1 %= 64;
		result = op2 >> (op1 % op1.GetBitCount());
		result |= op2 << (Data(size, op1.GetBitCount()) - (op1 % op1.GetBitCount()));
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		if(!op1.Zero())
		{
			cpu->SetC(op2.GetBit((op1 % op1.GetBitCount()).GetData() - 1));
		}
		else
		{
			cpu->SetC(false);
		}

		//Calculate the additional execution time
		ExecuteTime additionalCycles;
		if(target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT)
		{
			additionalCycles.Set(2 * op1.GetData(), 0, 0);
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		source.AddLabelTargetsToSet(labelTargetLocations);
		target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
