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
