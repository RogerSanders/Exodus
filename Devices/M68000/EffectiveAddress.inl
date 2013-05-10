namespace M68000 {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum EffectiveAddress::Mode
{
	DATAREG_DIRECT,             //ext 0
	ADDREG_DIRECT,              //ext 0

	ADDREG_INDIRECT,            //ext 0
	ADDREG_INDIRECT_POSTINC,    //ext 0
	ADDREG_INDIRECT_PREDEC,     //ext 0
	ADDREG_INDIRECT_DISPLACE,   //ext 1
	ADDREG_INDIRECT_INDEX_8BIT, //ext 1

	PC_INDIRECT_DISPLACE,       //ext 1
	PC_INDIRECT_INDEX_8BIT,     //ext 1

	ABS_WORD,                   //ext 1
	ABS_LONG,                   //ext 2

	IMMEDIATE,                  //ext 1-2
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
	mode = DATAREG_DIRECT;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressDirect(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = ADDREG_DIRECT;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirect(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = ADDREG_INDIRECT;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressPostinc(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = ADDREG_INDIRECT_POSTINC;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressPredec(Bitcount asize, const M68000Long& location, unsigned int areg)
{
	size = asize;
	savedPC = location;
	mode = ADDREG_INDIRECT_PREDEC;
	reg = areg;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirectDisplace(Bitcount asize, const M68000Long& location, unsigned int areg, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = ADDREG_INDIRECT_DISPLACE;
	reg = areg;

	displacement.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, displacement, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAddressIndirectIndex(Bitcount asize, const M68000Long& location, unsigned int areg, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = ADDREG_INDIRECT_INDEX_8BIT;
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
	mode = ABS_WORD;

	address.Resize(BITCOUNT_WORD);
	cpu->ReadMemory(location, address, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildAbsoluteAddressLong(Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = ABS_LONG;

	address.Resize(BITCOUNT_LONG);
	cpu->ReadMemory(location, address, cpu->GetFunctionCode(true), transparent, location, false, instructionRegister, false, false);
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildPCIndirectDisplace(Bitcount asize, const M68000Long& location, const M68000* cpu, bool transparent, const M68000Word& instructionRegister)
{
	size = asize;
	savedPC = location;
	mode = PC_INDIRECT_DISPLACE;

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
	savedPC = location;
	mode = PC_INDIRECT_INDEX_8BIT;

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
	mode = IMMEDIATE;

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
	mode = IMMEDIATE;
	data.Resize(size);
	data = adata;
	dataSignExtended = signExtended;
}

//----------------------------------------------------------------------------------------
void EffectiveAddress::BuildQuickData(const M68000Long& location, unsigned int adata)
{
	size = BITCOUNT_BYTE;
	savedPC = location;
	mode = IMMEDIATE;
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
		ReleaseAssert(false);
		break;
	case BITCOUNT_BYTE:
	case BITCOUNT_WORD:
		addressSize = extensionSize8[mode];
		break;
	case BITCOUNT_LONG:
		addressSize = extensionSize32[mode];
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
		ReleaseAssert(false);
		break;
	case BITCOUNT_BYTE:
	case BITCOUNT_WORD:
		executeTime = executeTime8[mode];
		break;
	case BITCOUNT_LONG:
		executeTime = executeTime32[mode];
	}
	return executeTime;
}

//----------------------------------------------------------------------------------------
EffectiveAddress::Mode EffectiveAddress::GetAddressMode() const
{
	return mode;
}

} //Close namespace M68000
