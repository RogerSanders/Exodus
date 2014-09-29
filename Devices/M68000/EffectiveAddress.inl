#include "Debug/Debug.pkg"
namespace M68000 {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class EffectiveAddress::Mode
{
	DataRegDirect = 0,       //ext 0
	AddRegDirect,            //ext 0

	AddRegIndirect,          //ext 0
	AddRegIndirectPostInc,   //ext 0
	AddRegIndirectPreDec,    //ext 0
	AddRegIndirectDisplace,  //ext 1
	AddRegIndirectIndex8Bit, //ext 1

	PCIndirectDisplace,      //ext 1
	PCIndirectIndex8Bit,     //ext 1

	ABSWord,                 //ext 1
	ABSLong,                 //ext 2

	Immediate,               //ext 1-2
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EffectiveAddress::EffectiveAddress()
:data(BITCOUNT_BYTE), displacement(BITCOUNT_BYTE), dataSignExtended(false)
{}

//----------------------------------------------------------------------------------------
//Decode functions
//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildDataDirect(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = Mode::DataRegDirect;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressDirect(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = Mode::AddRegDirect;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirect(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = Mode::AddRegIndirect;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressPostinc(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = Mode::AddRegIndirectPostInc;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressPredec(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = Mode::AddRegIndirectPreDec;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirectDisplace(Bitcount asize, const M68000Long& location, unsigned int areg, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = Mode::AddRegIndirectDisplace;
	reg = areg;

	displacement.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, displacement, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirectIndex(Bitcount asize, const M68000Long& location, unsigned int areg, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = Mode::AddRegIndirectIndex8Bit;
	reg = areg;

	M68000Word temp;
	cpu->ReadMemory(location, temp, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);

	//	-----------------------------------------------------------------
	//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//	|---|-----------|---|---|---|---|-------------------------------|
	//	|D/A|  Register |W/L| 0 | 0 | 0 |          Displacement         |
	//	-----------------------------------------------------------------
	useAddressRegister = temp.GetBit(15);
	if(!temp.GetBit(11))
	{
		indexSize = BITCOUNT_WORD;
	}
	else
	{
		indexSize = BITCOUNT_LONG;
	}
	indexReg = temp.GetDataSegment(12, 3);
	displacement.Resize(BITCOUNT_BYTE);
	temp.GetLowerBits(displacement);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddressWord(Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = Mode::ABSWord;

	address.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, address, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddressLong(Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = Mode::ABSLong;

	address.Resize(BITCOUNT_LONG);
	cpu->ReadMemory(location, address, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildPCIndirectDisplace(Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	mode = Mode::PCIndirectDisplace;

	//Note that all PC-relative address calculations work by taking the address of the
	//extension word which defines the effective address as the current value of the PC.
	//This is defined in the M68000 Programmers Manual, in the description of each of the
	//PC-relative addressing modes. From the manual: "The value of the PC is the address
	//of the extension word". We store the correct PC here at the time of decoding, so
	//that we can correctly decode PC-relative addresses during execution.
	savedPC = location;

	displacement.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, displacement, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildPCIndirectIndex(Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	mode = Mode::PCIndirectIndex8Bit;

	//Note that all PC-relative address calculations work by taking the address of the
	//extension word which defines the effective address as the current value of the PC.
	//This is defined in the M68000 Programmers Manual, in the description of each of the
	//PC-relative addressing modes. From the manual: "The value of the PC is the address
	//of the extension word". We store the correct PC here at the time of decoding, so
	//that we can correctly decode PC-relative addresses during execution.
	savedPC = location;

	M68000Word temp;
	cpu->ReadMemory(location, temp, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);

	//	-----------------------------------------------------------------
	//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//	|---|-----------|---|---|---|---|-------------------------------|
	//	|D/A|  Register |W/L| 0 | 0 | 0 |          Displacement         |
	//	-----------------------------------------------------------------
	useAddressRegister = temp.GetBit(15);
	if(!temp.GetBit(11))
	{
		indexSize = BITCOUNT_WORD;
	}
	else
	{
		indexSize = BITCOUNT_LONG;
	}
	indexReg = temp.GetDataSegment(12, 3);
	displacement.Resize(BITCOUNT_BYTE);
	temp.GetLowerBits(displacement);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = Mode::Immediate;

	data.Resize(size);
	M68000Long tempLocation = location;
	if(size == BITCOUNT_BYTE)
	{
		tempLocation += 1;
	}
	cpu->ReadMemory(tempLocation, data, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildImmediateData(const M68000Long& location, const Data& adata, bool signExtended)
{
	size = (Bitcount)data.GetBitCount();
	savedPC = location;
	mode = Mode::Immediate;
	data.Resize(size);
	data = adata;
	dataSignExtended = signExtended;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildQuickData(const M68000Long& location, unsigned int adata)
{
	size = BITCOUNT_BYTE;
	savedPC = location;
	mode = Mode::Immediate;
	data.Resize(size);
	if(adata == 0)
	{
		adata = 8;
	}
	data = adata;
}

//----------------------------------------------------------------------------------------
//Address functions
//----------------------------------------------------------------------------------------
M68000Long EffectiveAddress::ExtractProcessedImmediateData() const
{
	M68000Long processedImmediateData;
	processedImmediateData = data.SignExtend(processedImmediateData.GetBitCount()).GetData();
	return processedImmediateData;
}

//----------------------------------------------------------------------------------------
M68000Long EffectiveAddress::GetSavedPC() const
{
	return savedPC;
}

//----------------------------------------------------------------------------------------
bool EffectiveAddress::IncrementAddress(Bitcount asize)
{
	address += Data(asize).GetByteSize();
	return true;
}

//----------------------------------------------------------------------------------------
//Extension word info
//----------------------------------------------------------------------------------------
unsigned int EffectiveAddress::ExtensionSize()
{
	unsigned int addressSize = 0;
	switch(size)
	{
	default:
		DebugAssert(false);
		break;
	case BITCOUNT_BYTE:
	case BITCOUNT_WORD:
		addressSize = extensionSize8[(unsigned int)mode];
		break;
	case BITCOUNT_LONG:
		addressSize = extensionSize32[(unsigned int)mode];
	}
	return addressSize;
}

//----------------------------------------------------------------------------------------
ExecuteTime EffectiveAddress::DecodeTime()
{
	ExecuteTime executeTime;
	switch(size)
	{
	default:
		DebugAssert(false);
		break;
	case BITCOUNT_BYTE:
	case BITCOUNT_WORD:
		executeTime = executeTime8[(unsigned int)mode];
		break;
	case BITCOUNT_LONG:
		executeTime = executeTime32[(unsigned int)mode];
	}
	return executeTime;
}

//----------------------------------------------------------------------------------------
EffectiveAddress::Mode EffectiveAddress::GetAddressMode() const
{
	return mode;
}

} //Close namespace M68000
