//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct DataRemapTable::BitMapping
{
	unsigned int bitMaskOriginal;
	unsigned int bitMaskConverted;
};

//----------------------------------------------------------------------------------------
struct DataRemapTable::MappingElement
{
	bool forcedBitElement;
	bool forcedBitValue;
	unsigned int sourceDataBitNumber;
};

//----------------------------------------------------------------------------------------
//Data conversion functions
//----------------------------------------------------------------------------------------
unsigned int DataRemapTable::GetBitCountConverted() const
{
	return bitCountConverted;
}

//----------------------------------------------------------------------------------------
unsigned int DataRemapTable::GetBitMaskOriginalLinesPreserved() const
{
	return bitMaskOriginal;
}
