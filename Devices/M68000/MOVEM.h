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

	ExecuteTime GetExecuteTime(EffectiveAddress::Mode targetMode, bool memoryToRegisters, Bitcount size, unsigned int n)
	{
		static const ExecuteTime executeTimeMemoryToRegisters16[9] = {
			ExecuteTime(12+(4*n), (3+n), 0),       //(An)
			ExecuteTime(12+(4*n), (3+n), 0),       //(An)+
			ExecuteTime(0, 0, 0),                  //-(An)
			ExecuteTime(16+(4*n), (4+n), 0),       //d(An)
			ExecuteTime(18+(4*n), (4+n), 0),       //d(An,ix)+
			ExecuteTime(16+(4*n), (4+n), 0),       //xxx.W
			ExecuteTime(20+(4*n), (5+n), 0),       //xxx.L
			ExecuteTime(16+(4*n), (4+n), 0),       //d(PC)
			ExecuteTime(18+(4*n), (4+n), 0)};      //d(PC,ix)
		static const ExecuteTime executeTimeMemoryToRegisters32[9] = {
			ExecuteTime(12+(8*n), (3+(2*n)), 0),   //(An)
			ExecuteTime(12+(8*n), (3+n), 0),       //(An)+
			ExecuteTime(0, 0, 0),                  //-(An)
			ExecuteTime(16+(8*n), (4+(2*n)), 0),   //d(An)
			ExecuteTime(18+(8*n), (4+(2*n)), 0),   //d(An,ix)+
			ExecuteTime(16+(8*n), (4+(2*n)), 0),   //xxx.W
			ExecuteTime(20+(8*n), (5+(2*n)), 0),   //xxx.L
			ExecuteTime(16+(8*n), (4+(2*n)), 0),   //d(PC)
			ExecuteTime(18+(8*n), (4+(2*n)), 0)};  //d(PC,ix)
		static const ExecuteTime executeTimeRegistersToMemory16[9] = {
			ExecuteTime(8+(4*n), (2+n), 0),        //(An)
			ExecuteTime(0, 0, 0),                  //(An)+
			ExecuteTime(8+(4*n), (2+n), 0),        //-(An)
			ExecuteTime(12+(4*n), (3+n), 0),       //d(An)
			ExecuteTime(14+(4*n), (3+n), 0),       //d(An,ix)+
			ExecuteTime(12+(4*n), (3+n), 0),       //xxx.W
			ExecuteTime(16+(4*n), (4+n), 0),       //xxx.L
			ExecuteTime(0, 0, 0),                  //d(PC)
			ExecuteTime(0, 0, 0)};                 //d(PC,ix)
		static const ExecuteTime executeTimeRegistersToMemory32[9] = {
			ExecuteTime(8+(8*n), (2+n), 0),        //(An)
			ExecuteTime(0, 0, 0),                  //(An)+
			ExecuteTime(8+(8*n), (2+n), 0),        //-(An)
			ExecuteTime(12+(8*n), (3+n), 0),       //d(An)
			ExecuteTime(14+(8*n), (3+n), 0),       //d(An,ix)+
			ExecuteTime(12+(8*n), (3+n), 0),       //xxx.W
			ExecuteTime(16+(8*n), (4+n), 0),       //xxx.L
			ExecuteTime(0, 0, 0),                  //d(PC)
			ExecuteTime(0, 0, 0)};                 //d(PC,ix)

		unsigned int targetIndex = 0;
		switch(targetMode)
		{
		case EffectiveAddress::ADDREG_INDIRECT:
			targetIndex = 0;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_POSTINC:
			targetIndex = 1;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_PREDEC:
			targetIndex = 2;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_DISPLACE:
			targetIndex = 3;
			break;
		case EffectiveAddress::ADDREG_INDIRECT_INDEX_8BIT:
			targetIndex = 4;
			break;
		case EffectiveAddress::ABS_WORD:
			targetIndex = 5;
			break;
		case EffectiveAddress::ABS_LONG:
			targetIndex = 6;
			break;
		case EffectiveAddress::PC_INDIRECT_DISPLACE:
			targetIndex = 7;
			break;
		case EffectiveAddress::PC_INDIRECT_INDEX_8BIT:
			targetIndex = 8;
			break;
		}

		if(memoryToRegisters)
		{
			if(size == BITCOUNT_LONG)
			{
				return executeTimeMemoryToRegisters32[targetIndex];
			}
			else
			{
				return executeTimeMemoryToRegisters16[targetIndex];
			}
		}
		else
		{
			if(size == BITCOUNT_LONG)
			{
				return executeTimeRegistersToMemory32[targetIndex];
			}
			else
			{
				return executeTimeRegistersToMemory16[targetIndex];
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
		for(unsigned int i = 0; i < mask.GetBitCount(); ++i)
		{
			bool selected = mask.GetBit(i);
			bool addressRegister = (i >= 8);
			unsigned int reg = (i % 8);
			if(addressMode == MODE_PREDECREMENT)
			{
				//If we're using the predec addressing mode, reverse the mask.
				addressRegister = !addressRegister;
				reg = 7 - reg;
			}

			if(selected)
			{
				//Add the register to the mask disassembly
				if(!maskDisassembly.str().empty())
				{
					maskDisassembly << L'/';
				}
				if(addressRegister)
				{
					maskDisassembly << L'A' << reg;
				}
				else
				{
					maskDisassembly << L'D' << reg;
				}
			}
		}

		std::wstring targetDisassembly = target.Disassemble(labelSettings);
		if(addressMode == MODE_PREDECREMENT)
		{
			targetDisassembly = L"-(" + target.Disassemble(labelSettings) + L")";
		}
		else if(addressMode == MODE_POSTINCREMENT)
		{
			targetDisassembly = L"(" + target.Disassemble(labelSettings) + L")+";
		}

		std::wstring argumentDisassembly;
		if(memoryToRegisters)
		{
			argumentDisassembly = targetDisassembly + L", " + maskDisassembly.str();
		}
		else
		{
			argumentDisassembly = maskDisassembly.str() + L", " + targetDisassembly;
		}
		return Disassembly(GetOpcodeName() + L"." + DisassembleSize(size), argumentDisassembly);
	}

	virtual void M68000Decode(const M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	                                         |----------<ea>---------|
//	-----------------------------------------=========================
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6  | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|---|---|---|---|---|----|-----------|-----------|
//	| 0 | 1 | 0 | 0 | 1 |dr | 0 | 0 | 1 |SIZE|    MODE   | REGISTER  |
//	|----------------------------------------------------------------|
//	|                    MASK FROM REGISTER LIST                     |
//	------------------------------------------------------------------
		if(!data.GetBit(6))
		{
			size = BITCOUNT_WORD;
		}
		else
		{
			size = BITCOUNT_LONG;
		}
		cpu->ReadMemory(location + GetInstructionSize(), mask, cpu->GetFunctionCode(true), transparent, location + GetInstructionSize(), true, GetInstructionRegister(), false, false);
		AddInstructionSize(mask.GetByteSize());

		memoryToRegisters = (data.GetBit(10));

		//Override postinc and predec address modes. We do some trickery under these
		//address modes to deal with cases where the address register involved is also
		//saved or loaded as part of the move.
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		switch(target.GetAddressMode())
		{
		case EffectiveAddress::ADDREG_INDIRECT_PREDEC:
			addressMode = MODE_PREDECREMENT;
			target.BuildAddressDirect(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			break;
		case EffectiveAddress::ADDREG_INDIRECT_POSTINC:
			addressMode = MODE_POSTINCREMENT;
			target.BuildAddressDirect(size, location + GetInstructionSize(), data.GetDataSegment(0, 3));
			break;
		default:
			addressMode = MODE_FIXED;
			break;
		}
		AddInstructionSize(target.ExtensionSize());

		//Resolve the register mask
		for(unsigned int i = 0; i < mask.GetBitCount(); ++i)
		{
			bool selected = mask.GetBit(i);
			bool addressRegister = (i >= 8);
			unsigned int reg = (i % 8);
			if(addressMode == MODE_PREDECREMENT)
			{
				//If we're using the predec addressing mode, reverse the mask.
				addressRegister = !addressRegister;
				reg = 7 - reg;
			}

			if(selected)
			{
				//Add the register to our list of target registers
				EffectiveAddress targetRegister;
				if(addressRegister)
				{
					targetRegister.BuildAddressDirect(size, location + GetInstructionSize(), reg);
				}
				else
				{
					targetRegister.BuildDataDirect(size, location + GetInstructionSize(), reg);
				}
				registers.push_back(targetRegister);
			}
		}

		//Calculate the execution time
		if(addressMode == MODE_PREDECREMENT)
		{
			AddExecuteCycleCount(GetExecuteTime(EffectiveAddress::ADDREG_INDIRECT_PREDEC, memoryToRegisters, size, (unsigned int)registers.size()));
		}
		else if(addressMode == MODE_POSTINCREMENT)
		{
			AddExecuteCycleCount(GetExecuteTime(EffectiveAddress::ADDREG_INDIRECT_POSTINC, memoryToRegisters, size, (unsigned int)registers.size()));
		}
		else
		{
			AddExecuteCycleCount(GetExecuteTime(target.GetAddressMode(), memoryToRegisters, size, (unsigned int)registers.size()));
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		//Resolve the effective address and work with the address directly. We do this so
		//that if an address register is being used in the address calculation, and that
		//same register is being saved or loaded as part of the operation, we can ensure
		//the operation behaves as it should. Refer to the M68000 Programmer's Guide
		//section 4, page 128, paragraph 4, for more info.
		EffectiveAddress memory;
		M68000Long address;
		if(target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT)
		{
			additionalTime += target.Read(cpu, address, GetInstructionRegister());
		}
		else
		{
			target.GetAddress(cpu, address);
		}

		//Loop through the list of registers and perform the operation on each entry
		for(std::list<EffectiveAddress>::const_iterator i = registers.begin(); i != registers.end(); ++i)
		{
			if(memoryToRegisters)
			{
				Data temp(size);

				//Record active disassembly info for this register move
				if(cpu->ActiveDisassemblyEnabled())
				{
					M68000::LabelSubstitutionSettings labelSettings;
					labelSettings.enableSubstitution = false;
					cpu->AddDisassemblyAddressInfoData(address.GetData(), temp.GetByteSize(), M68000::DISASSEMBLYDATATYPE_INTEGER, 0, i->Disassemble(labelSettings));
				}

				//Read the register data from memory and load it into the register
				additionalTime += cpu->ReadMemory(address, temp, cpu->GetFunctionCode(false), location + 2, true, GetInstructionRegister(), false, false);
				additionalTime += i->Write(cpu, temp.SignExtend(BITCOUNT_LONG), GetInstructionRegister(), false, false, true, address.GetData(), temp.GetByteSize());
				address += temp.GetByteSize();
			}
			else
			{
				//Write the register data into memory
				Data temp(size);
				additionalTime += i->Read(cpu, temp, GetInstructionRegister());
				if(addressMode == MODE_PREDECREMENT)	address -= temp.GetByteSize();
				additionalTime += cpu->WriteMemory(address, temp, cpu->GetFunctionCode(false), location + 2, true, GetInstructionRegister(), false, false);
				if(addressMode != MODE_PREDECREMENT)	address += temp.GetByteSize();
			}
		}

		//If the address mode was predec or postinc, write the updated address to the
		//register.
		if(target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT)
		{
			target.Write(cpu, address, GetInstructionRegister());
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

	virtual void GetLabelTargetLocations(std::set<unsigned int>& labelTargetLocations) const
	{
		target.AddLabelTargetsToSet(labelTargetLocations);
	}

private:
	std::list<EffectiveAddress> registers;
	EffectiveAddress target;
	Bitcount size;
	M68000Word mask;
	bool memoryToRegisters;
	AddressMode addressMode;
};

} //Close namespace M68000
#endif
