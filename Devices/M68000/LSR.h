#include "M68000Instruction.h"
#ifndef __M68000_LSR_H__
#define __M68000_LSR_H__
namespace M68000 {

class LSR:public M68000Instruction
{
public:
	virtual LSR* Clone() const {return new LSR();}
	virtual LSR* ClonePlacement(void* buffer) const {return new(buffer) LSR();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"1110***0CC*01***", L"CC=00-10");
		result &= table.AllocateRegionToOpcode(this, L"1110001011DDDDDD", L"DDDDDD=010000-110111,111000,111001");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"LSR";
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

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	In the case of the shifting of a register:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|---|---|---|-----------|
//	| 1 | 1 | 1 | 0 |  NUMBER/  | 0 |  SIZE |i/r| 0 | 0 | REGISTER  |
//	|   |   |   |   |  REGISTER |   |       |   |   |   |           |
//	-----------------------------------------------------------------
		if (data.GetDataSegment(6, 2) != 3)
		{
			switch (data.GetDataSegment(6, 2))
			{
			case 0:	// 00
				_size = BITCOUNT_BYTE;
				break;
			case 1:	// 01
				_size = BITCOUNT_WORD;
				break;
			case 2:	// 10
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
//	In the case of the shifting of a memory area:
//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|---|-----------|-----------|
//	| 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 1 |    MODE   | REGISTER  |
//	----------------------------------------=========================
//	                                                  <ea>
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

		// Perform the operation
		additionalTime += _source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += _target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		op1 %= 64;
		//##NOTE## Doing a shift on an x86 platform effectively does a modulus by 32 to
		// the shift count before performing the operation. Since the M68000 platform
		// works up to 64, we zero the _target operand to simulate a shift greater than
		// 31 places.
		if (op1.GetData() >= 32)
		{
			op2 = 0;
		}
		result = op2 >> op1;
		additionalTime += _target.Write(cpu, result, GetInstructionRegister());

		// Set the flag results
		cpu->SetN(result.Negative());
		cpu->SetZ(result.Zero());
		cpu->SetV(false);
		if (!op1.Zero())
		{
			if (op1.GetData() > op2.GetBitCount())
			{
				cpu->SetX(false);
				cpu->SetC(false);
			}
			else
			{
				cpu->SetX(op2.GetBit(op1.GetData() - 1));
				cpu->SetC(op2.GetBit(op1.GetData() - 1));
			}
		}
		else
		{
			cpu->SetC(false);
		}

		// Calculate the additional execution time
		ExecuteTime additionalCycles;
		if (_target.GetAddressMode() == EffectiveAddress::Mode::DataRegDirect)
		{
			additionalCycles.Set(2 * op1.GetData(), 0, 0);
		}

		// Adjust the PC and return the execution time
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

} // Close namespace M68000
#endif
