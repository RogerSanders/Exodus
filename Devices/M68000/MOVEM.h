#include "M68000Instruction.h"
#ifndef __M68000_MOVEM_H__
#define __M68000_MOVEM_H__
#include <list>
namespace M68000 {

class MOVEM :public M68000Instruction
{
public:
	enum AddressMode
	{
		MODE_PREDECREMENT,
		MODE_POSTINCREMENT,
		MODE_FIXED
	};

	ExecuteTime GetExecuteTime(EffectiveAddress::Mode _targetMode, bool operationMemoryToRegisters, Bitcount operationSize, unsigned int n)
	{
		static const ExecuteTime executeTimeMemoryToRegisters16[9] = {
			ExecuteTime(12+(4*n), (3+n), 0),       //(An)
			ExecuteTime(12+(4*n), (3+n), 0),       //(An)+
			ExecuteTime(0, 0, 0),                  // -(An)
			ExecuteTime(16+(4*n), (4+n), 0),       // d(An)
			ExecuteTime(18+(4*n), (4+n), 0),       // d(An,ix)+
			ExecuteTime(16+(4*n), (4+n), 0),       // xxx.W
			ExecuteTime(20+(4*n), (5+n), 0),       // xxx.L
			ExecuteTime(16+(4*n), (4+n), 0),       // d(PC)
			ExecuteTime(18+(4*n), (4+n), 0)};      // d(PC,ix)
		static const ExecuteTime executeTimeMemoryToRegisters32[9] = {
			ExecuteTime(12+(8*n), (3+(2*n)), 0),   //(An)
			ExecuteTime(12+(8*n), (3+n), 0),       //(An)+
			ExecuteTime(0, 0, 0),                  // -(An)
			ExecuteTime(16+(8*n), (4+(2*n)), 0),   // d(An)
			ExecuteTime(18+(8*n), (4+(2*n)), 0),   // d(An,ix)+
			ExecuteTime(16+(8*n), (4+(2*n)), 0),   // xxx.W
			ExecuteTime(20+(8*n), (5+(2*n)), 0),   // xxx.L
			ExecuteTime(16+(8*n), (4+(2*n)), 0),   // d(PC)
			ExecuteTime(18+(8*n), (4+(2*n)), 0)};  // d(PC,ix)
		static const ExecuteTime executeTimeRegistersToMemory16[9] = {
			ExecuteTime(8+(4*n), (2+n), 0),        //(An)
			ExecuteTime(0, 0, 0),                  //(An)+
			ExecuteTime(8+(4*n), (2+n), 0),        // -(An)
			ExecuteTime(12+(4*n), (3+n), 0),       // d(An)
			ExecuteTime(14+(4*n), (3+n), 0),       // d(An,ix)+
			ExecuteTime(12+(4*n), (3+n), 0),       // xxx.W
			ExecuteTime(16+(4*n), (4+n), 0),       // xxx.L
			ExecuteTime(0, 0, 0),                  // d(PC)
			ExecuteTime(0, 0, 0)};                 // d(PC,ix)
		static const ExecuteTime executeTimeRegistersToMemory32[9] = {
			ExecuteTime(8+(8*n), (2+n), 0),        //(An)
			ExecuteTime(0, 0, 0),                  //(An)+
			ExecuteTime(8+(8*n), (2+n), 0),        // -(An)
			ExecuteTime(12+(8*n), (3+n), 0),       // d(An)
			ExecuteTime(14+(8*n), (3+n), 0),       // d(An,ix)+
			ExecuteTime(12+(8*n), (3+n), 0),       // xxx.W
			ExecuteTime(16+(8*n), (4+n), 0),       // xxx.L
			ExecuteTime(0, 0, 0),                  // d(PC)
			ExecuteTime(0, 0, 0)};                 // d(PC,ix)

		unsigned int _targetIndex = 0;
		switch (_targetMode)
		{
		case EffectiveAddress::Mode::AddRegIndirect:
			_targetIndex = 0;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPostInc:
			_targetIndex = 1;
			break;
		case EffectiveAddress::Mode::AddRegIndirectPreDec:
			_targetIndex = 2;
			break;
		case EffectiveAddress::Mode::AddRegIndirectDisplace:
			_targetIndex = 3;
			break;
		case EffectiveAddress::Mode::AddRegIndirectIndex8Bit:
			_targetIndex = 4;
			break;
		case EffectiveAddress::Mode::ABSWord:
			_targetIndex = 5;
			break;
		case EffectiveAddress::Mode::ABSLong:
			_targetIndex = 6;
			break;
		case EffectiveAddress::Mode::PCIndirectDisplace:
			_targetIndex = 7;
			break;
		case EffectiveAddress::Mode::PCIndirectIndex8Bit:
			_targetIndex = 8;
			break;
		}

		if (operationMemoryToRegisters)
		{
			if (operationSize == BITCOUNT_LONG)
			{
				return executeTimeMemoryToRegisters32[_targetIndex];
			}
			else
			{
				return executeTimeMemoryToRegisters16[_targetIndex];
			}
		}
		else
		{
			if (operationSize == BITCOUNT_LONG)
			{
				return executeTimeRegistersToMemory32[_targetIndex];
			}
			else
			{
				return executeTimeRegistersToMemory16[_targetIndex];
			}
		}
	}

	virtual MOVEM* Clone() const {return new MOVEM();}
	virtual MOVEM* ClonePlacement(void* buffer) const {return new(buffer) MOVEM();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>& table) const
	{
		bool result = true;
		result &= table.AllocateRegionToOpcode(this, L"01001B001*DDDDDD", L"B=0 DDDDDD=010000-010111,100000-110111,111000,111001");
		result &= table.AllocateRegionToOpcode(this, L"01001B001*DDDDDD", L"B=1 DDDDDD=010000-011111,101000-110111,111000,111001,111010,111011");
		return result;
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"MOVEM";
	}

	virtual Disassembly M68000Disassemble(const M68000::LabelSubstitutionSettings& labelSettings) const
	{
		std::wstringstream maskDisassembly;
		for (unsigned int i = 0; i < _mask.GetBitCount(); ++i)
		{
			bool selected = _mask.GetBit(i);
			bool addressRegister = (i >= 8);
			unsigned int reg = (i % 8);
			if (_addressMode == MODE_PREDECREMENT)
			{
				// If we're using the predec addressing mode, reverse the mask.
				addressRegister = !addressRegister;
				reg = 7 - reg;
			}

			if (selected)
			{
				// Add the register to the mask disassembly
				if (!maskDisassembly.str().empty())
				{
					maskDisassembly << L'/';
				}
				if (addressRegister)
				{
					maskDisassembly << L'A' << reg;
				}
				else
				{
					maskDisassembly << L'D' << reg;
				}
			}
		}

		std::wstring _targetDisassembly = _target.Disassemble(labelSettings);
		if (_addressMode == MODE_PREDECREMENT)
		{
			_targetDisassembly = L"-(" + _target.Disassemble(labelSettings) + L")";
		}
		else if (_addressMode == MODE_POSTINCREMENT)
		{
			_targetDisassembly = L"(" + _target.Disassemble(labelSettings) + L")+";
		}

		std::wstring argumentDisassembly;
		if (_memoryToRegisters)
		{
			argumentDisassembly = _targetDisassembly + L", " + maskDisassembly.str();
		}
		else
		{
			argumentDisassembly = maskDisassembly.str() + L", " + _targetDisassembly;
		}
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(_size), argumentDisassembly);
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	                                         |----------<ea>---------|
//	-----------------------------------------=========================
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6  | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|----|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 1 |dr | 0 | 0 | 1 |SIZE|    MODE   | REGISTER  |
//	|----------------------------------------------------------------|
//	|                    MASK FROM REGISTER LIST                     |
//	------------------------------------------------------------------
		if (!data.GetBit(6))
		{
			_size = BITCOUNT_WORD;
		}
		else
		{
			_size = BITCOUNT_LONG;
		}
		cpu->ReadMemory(location + GetInstructionSize(), _mask, cpu->GetFunctionCode(true), transparent, location + GetInstructionSize(), true, GetInstructionRegister(), false, false);
		AddInstructionSize(_mask.GetByteSize());

		_memoryToRegisters = (data.GetBit(10));

		// Override postinc and predec address modes. We do some trickery under these
		// address modes to deal with cases where the address register involved is also
		// saved or loaded as part of the move.
		_target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), _size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		switch (_target.GetAddressMode())
		{
		case EffectiveAddress::Mode::AddRegIndirectPreDec:
			_addressMode = MODE_PREDECREMENT;
			_target.BuildAddressDirect(_size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			break;
		case EffectiveAddress::Mode::AddRegIndirectPostInc:
			_addressMode = MODE_POSTINCREMENT;
			_target.BuildAddressDirect(_size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			break;
		default:
			_addressMode = MODE_FIXED;
			break;
		}
		AddInstructionSize(_target.ExtensionSize());

		// Resolve the register mask
		_registers.reserve(_mask.GetBitCount());
		for (unsigned int i = 0; i < _mask.GetBitCount(); ++i)
		{
			bool selected = _mask.GetBit(i);
			bool addressRegister = (i >= 8);
			unsigned int reg = (i % 8);
			if (_addressMode == MODE_PREDECREMENT)
			{
				// If we're using the predec addressing mode, reverse the mask.
				addressRegister = !addressRegister;
				reg = 7 - reg;
			}

			if (selected)
			{
				// Add the register to our list of _target registers
				EffectiveAddress _targetRegister;
				if (addressRegister)
				{
					_targetRegister.BuildAddressDirect(_size, location + GetInstructionSize(), reg);
				}
				else
				{
					_targetRegister.BuildDataDirect(_size, location + GetInstructionSize(), reg);
				}
				_registers.push_back(_targetRegister);
			}
		}

		// Calculate the execution time
		if (_addressMode == MODE_PREDECREMENT)
		{
			AddExecuteCycleCount(GetExecuteTime(EffectiveAddress::Mode::AddRegIndirectPreDec, _memoryToRegisters, _size, (unsigned int)_registers.size()));
		}
		else if (_addressMode == MODE_POSTINCREMENT)
		{
			AddExecuteCycleCount(GetExecuteTime(EffectiveAddress::Mode::AddRegIndirectPostInc, _memoryToRegisters, _size, (unsigned int)_registers.size()));
		}
		else
		{
			AddExecuteCycleCount(GetExecuteTime(_target.GetAddressMode(), _memoryToRegisters, _size, (unsigned int)_registers.size()));
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		// Resolve the effective address and work with the address directly. We do this so
		// that if an address register is being used in the address calculation, and that
		// same register is being saved or loaded as part of the operation, we can ensure
		// the operation behaves as it should. Refer to the M68000 Programmer's Guide
		// section 4, page 128, paragraph 4, for more info.
		EffectiveAddress memory;
		M68000Long address;
		if (_target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect)
		{
			additionalTime += _target.Read(cpu, address, GetInstructionRegister());
		}
		else
		{
			_target.GetAddress(cpu, address);
		}

		// Loop through the list of registers and perform the operation on each entry
		for (std::vector<EffectiveAddress>::const_iterator i = _registers.begin(); i != _registers.end(); ++i)
		{
			if (_memoryToRegisters)
			{
				Data temp(_size);

				// Record active disassembly info for this register move
				if (cpu->ActiveDisassemblyEnabled())
				{
					M68000::LabelSubstitutionSettings labelSettings;
					labelSettings.enableSubstitution = false;
					cpu->AddDisassemblyAddressInfoData(address.GetData(), temp.GetByteSize(), M68000::DisassemblyDataType::Integer, 0, i->Disassemble(labelSettings));
				}

				// Read the register data from memory and load it into the register
				additionalTime += cpu->ReadMemory(address, temp, cpu->GetFunctionCode(false), location + 2, true, GetInstructionRegister(), false, false);
				additionalTime += i->Write(cpu, temp.SignExtend(BITCOUNT_LONG), GetInstructionRegister(), false, false, true, address.GetData(), temp.GetByteSize());
				address += temp.GetByteSize();
			}
			else
			{
				// Write the register data into memory
				Data temp(_size);
				additionalTime += i->Read(cpu, temp, GetInstructionRegister());
				if (_addressMode == MODE_PREDECREMENT)	address -= temp.GetByteSize();
				additionalTime += cpu->WriteMemory(address, temp, cpu->GetFunctionCode(false), location + 2, true, GetInstructionRegister(), false, false);
				if (_addressMode != MODE_PREDECREMENT)	address += temp.GetByteSize();
			}
		}

		// If the address mode was predec or postinc, write the updated address to the
		// register.
		if (_target.GetAddressMode() == EffectiveAddress::Mode::AddRegDirect)
		{
			_target.Write(cpu, address, GetInstructionRegister());
		}

		// Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		_target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	std::vector<EffectiveAddress> _registers;
	EffectiveAddress _target;
	Bitcount _size;
	M68000Word _mask;
	bool _memoryToRegisters;
	AddressMode _addressMode;
};

} // Close namespace M68000
#endif
