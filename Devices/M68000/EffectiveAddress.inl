#include "Debug/Debug.pkg"
namespace M68000 {

//----------------------------------------------------------------------------------------------------------------------
// Enumerations
//----------------------------------------------------------------------------------------------------------------------
enum class EffectiveAddress::Mode
{
	DataRegDirect = 0,       // ext 0
	AddRegDirect,            // ext 0

	AddRegIndirect,          // ext 0
	AddRegIndirectPostInc,   // ext 0
	AddRegIndirectPreDec,    // ext 0
	AddRegIndirectDisplace,  // ext 1
	AddRegIndirectIndex8Bit, // ext 1

	PCIndirectDisplace,      // ext 1
	PCIndirectIndex8Bit,     // ext 1

	ABSWord,                 // ext 1
	ABSLong,                 // ext 2

	Immediate,               // ext 1-2
};

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
EffectiveAddress::EffectiveAddress()
:_data(BITCOUNT_BYTE), _displacement(BITCOUNT_BYTE), _dataSignExtended(false)
{}

//----------------------------------------------------------------------------------------------------------------------
// Decode functions
//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildDataDirect(Bitcount size, const M68000Long& location, unsigned int reg)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::DataRegDirect;
	_reg = reg;
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressDirect(Bitcount size, const M68000Long& location, unsigned int reg)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::AddRegDirect;
	_reg = reg;
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirect(Bitcount size, const M68000Long& location, unsigned int reg)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::AddRegIndirect;
	_reg = reg;
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressPostinc(Bitcount size, const M68000Long& location, unsigned int reg)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::AddRegIndirectPostInc;
	_reg = reg;
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressPredec(Bitcount size, const M68000Long& location, unsigned int reg)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::AddRegIndirectPreDec;
	_reg = reg;
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirectDisplace(Bitcount size, const M68000Long& location, unsigned int reg, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::AddRegIndirectDisplace;
	_reg = reg;

	_displacement.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, _displacement, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirectIndex(Bitcount size, const M68000Long& location, unsigned int reg, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::AddRegIndirectIndex8Bit;
	_reg = reg;

	M68000Word temp;
	cpu->ReadMemory(location, temp, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);

	//	-----------------------------------------------------------------
	//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//	|---|-----------|---|---|---|---|-------------------------------|
	//	|D/A|  Register |W/L| 0 | 0 | 0 |          Displacement         |
	//	-----------------------------------------------------------------
	_useAddressRegister = temp.GetBit(15);
	if (!temp.GetBit(11))
	{
		_indexSize = BITCOUNT_WORD;
	}
	else
	{
		_indexSize = BITCOUNT_LONG;
	}
	_indexReg = temp.GetDataSegment(12, 3);
	_displacement.Resize(BITCOUNT_BYTE);
	temp.GetLowerBits(_displacement);
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddressWord(Bitcount size, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::ABSWord;

	_address.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, _address, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddressLong(Bitcount size, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::ABSLong;

	_address.Resize(BITCOUNT_LONG);
	cpu->ReadMemory(location, _address, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildPCIndirectDisplace(Bitcount size, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_size = size;
	_mode = Mode::PCIndirectDisplace;

	// Note that all PC-relative address calculations work by taking the address of the
	// extension word which defines the effective address as the current value of the PC.
	// This is defined in the M68000 Programmers Manual, in the description of each of the
	// PC-relative addressing modes. From the manual: "The value of the PC is the address
	// of the extension word". We store the correct PC here at the time of decoding, so
	// that we can correctly decode PC-relative addresses during execution.
	_savedPC = location;

	_displacement.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, _displacement, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildPCIndirectIndex(Bitcount size, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_size = size;
	_mode = Mode::PCIndirectIndex8Bit;

	// Note that all PC-relative address calculations work by taking the address of the
	// extension word which defines the effective address as the current value of the PC.
	// This is defined in the M68000 Programmers Manual, in the description of each of the
	// PC-relative addressing modes. From the manual: "The value of the PC is the address
	// of the extension word". We store the correct PC here at the time of decoding, so
	// that we can correctly decode PC-relative addresses during execution.
	_savedPC = location;

	M68000Word temp;
	cpu->ReadMemory(location, temp, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);

	//	-----------------------------------------------------------------
	//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//	|---|-----------|---|---|---|---|-------------------------------|
	//	|D/A|  Register |W/L| 0 | 0 | 0 |          Displacement         |
	//	-----------------------------------------------------------------
	_useAddressRegister = temp.GetBit(15);
	if (!temp.GetBit(11))
	{
		_indexSize = BITCOUNT_WORD;
	}
	else
	{
		_indexSize = BITCOUNT_LONG;
	}
	_indexReg = temp.GetDataSegment(12, 3);
	_displacement.Resize(BITCOUNT_BYTE);
	temp.GetLowerBits(_displacement);
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(Bitcount size, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	_size = size;
	_savedPC = location;
	_mode = Mode::Immediate;

	_data.Resize(_size);
	M68000Long tempLocation = location;
	if (_size == BITCOUNT_BYTE)
	{
		tempLocation += 1;
	}
	cpu->ReadMemory(tempLocation, _data, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(const M68000Long& location, const Data& data, bool signExtended)
{
	_size = (Bitcount)_data.GetBitCount();
	_savedPC = location;
	_mode = Mode::Immediate;
	_data.Resize(_size);
	_data = data;
	_dataSignExtended = signExtended;
}

//----------------------------------------------------------------------------------------------------------------------
void EffectiveAddress::BuildQuickData(const M68000Long& location, unsigned int data)
{
	_size = BITCOUNT_BYTE;
	_savedPC = location;
	_mode = Mode::Immediate;
	_data.Resize(_size);
	if (data == 0)
	{
		data = 8;
	}
	_data = data;
}

//----------------------------------------------------------------------------------------------------------------------
// Address functions
//----------------------------------------------------------------------------------------------------------------------
M68000Long EffectiveAddress::ExtractProcessedImmediateData() const
{
	M68000Long processedImmediateData;
	processedImmediateData = _data.SignExtend(processedImmediateData.GetBitCount()).GetData();
	return processedImmediateData;
}

//----------------------------------------------------------------------------------------------------------------------
M68000Long EffectiveAddress::GetSavedPC() const
{
	return _savedPC;
}

//----------------------------------------------------------------------------------------------------------------------
bool EffectiveAddress::IncrementAddress(Bitcount size)
{
	_address += Data(size).GetByteSize();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Extension word info
//----------------------------------------------------------------------------------------------------------------------
unsigned int EffectiveAddress::ExtensionSize()
{
	unsigned int addressSize = 0;
	switch (_size)
	{
	default:
		DebugAssert(false);
		break;
	case BITCOUNT_BYTE:
	case BITCOUNT_WORD:
		addressSize = ExtensionSize8[(unsigned int)_mode];
		break;
	case BITCOUNT_LONG:
		addressSize = ExtensionSize32[(unsigned int)_mode];
	}
	return addressSize;
}

//----------------------------------------------------------------------------------------------------------------------
ExecuteTime EffectiveAddress::DecodeTime()
{
	ExecuteTime executeTime;
	switch (_size)
	{
	default:
		DebugAssert(false);
		break;
	case BITCOUNT_BYTE:
	case BITCOUNT_WORD:
		executeTime = ExecuteTime8[(unsigned int)_mode];
		break;
	case BITCOUNT_LONG:
		executeTime = ExecuteTime32[(unsigned int)_mode];
	}
	return executeTime;
}

//----------------------------------------------------------------------------------------------------------------------
EffectiveAddress::Mode EffectiveAddress::GetAddressMode() const
{
	return _mode;
}

} // Close namespace M68000
