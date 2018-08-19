#include "M68000Instruction.h"
#ifndef __M68000_ROXR_H__
#define __M68000_ROXR_H__
namespace M68000 {

class ROXR :public M68000Instruction
{
public:
	virtual ROXR* Clone() const {return new ROXR();}
	virtual ROXR* ClonePlacement(void* buffer) const {return new(buffer) ROXR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1110***0CC*10***", L"CC=00-10");
		result &= table.AllocateRegionToOpcode(this, L"1110010011DDDDDD", L"DDDDDD=010000-110111,111000,111001");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"ROXR";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		if (_target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect)
		{
			return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), _source.Disassemble(labelSettings) + L", " + _target.Disassemble(labelSettings));
		}
		else
		{
			return Disassembly(GetOpcodeName(), _target.Disassemble(labelSettings));
		}
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	In the case of the rotating of a register:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|---|---|---|-----------|
//	| 1 | 1 | 1 | 0 |  NUMBER/  | 0 |  SIZE |i/r| 1 | 0 | REGISTER  |
//	|   |   |   |   |  REGISTER |   |       |   |   |   |           |
//	-----------------------------------------------------------------
		if (data.GetDataSegment(6, 2) != 3)
		{
			switch (data.GetDataSegment(6, 2))
			{
			case 0:	//00
				_size = BITCOUNT_BYTE;
				break;
			case 1:	//01
				_size = BITCOUNT_WORD;
				break;
			case 2:	//10
				_size = BITCOUNT_LONG;
				break;
			}

			_target.BuildDataDirect(_size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			if (!data.GetBit(5))
			{
				_source.BuildQuickData(location + GetInstructionSize(), data.GetDataSegment(9, 3));
			}
			else
			{
				_source.BuildDataDirect(_size, location + GetInstructionSize(), data.GetDataSegment(9, 3));
			}

			if (_size == BITCOUNT_LONG)
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
//	| 1 | 1 | 1 | 0 | 0 | 1 | 0 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                        |----------<ea>---------|
		{
			_size = BITCOUNT_WORD;
			_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
			AddInstructionSize(_target.ExtensionSize());
			_source.BuildImmediateData(location + GetInstructionSize(), M68000Word(1));

			AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			AddExecuteCycleCount(_target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(_size);
		Data op2(_size);
		Data result(_size);

		//Perform the operation. We do this one manually. It would be possible to avoid
		//the loop with a clever algorithm, but I really don't think it's worth it. The
		//X flag makes the operation quite complex, and I prefer this to a few tortured
		//lines of chained binary operators.
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		op1 %= 64;
		result = op2;
		for (unsigned int i = 0; i < op1.GetData(); ++i)
		{
			bool lastXBit = cpu->GetX();
			cpu->SetX(result.GetBit(0));
			cpu->SetC(result.GetBit(0));
			result >>= 1;
			result.SetBit(result.GetBitCount() - 1, lastXBit);
		}
		additionalTime += _target.Write(cpu, result, GetInstructionRegister());

		//Set the flag results. Note that X and C are also set in the operation above.
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		if (op1.Zero())
		{
			cpu->SetC(cpu->GetX());
		}

		//Calculate the additional execution time
		ExecuteTime additionalCycles;
		if (_target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect)
		{
			additionalCycles.Set(2 * op1.GetData(), 0, 0);
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime) + additionalCycles;
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		_source.AddLabelTargetsToSet(labelTargetLocations);
		_target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	EffectiveAddress _source;
	EffectiveAddress _target;
	Bitcount _size;
};

} //Close namespace M68000
#endif
