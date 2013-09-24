#include "DataRemapTable.h"
#include "DataConversion/DataConversion.pkg"
#include <list>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DataRemapTable::DataRemapTable()
:conversionTableStateSetManually(false)
{
	//Set the default maximum bit counts for our conversion tables to 20 bits, which gives
	//us a maximum table size of 1 MegaByte.
	conversionTableToMaxBitCount = 20;
	conversionTableFromMaxBitCount = 20;
}

//----------------------------------------------------------------------------------------
//Data conversion settings
//----------------------------------------------------------------------------------------
void DataRemapTable::SetConversionTableState(bool auseConversionTableTo, bool auseConversionTableFrom)
{
	useMethodConversionTableTo = auseConversionTableTo;
	useMethodConversionTableFrom = auseConversionTableFrom;
	conversionTableStateSetManually = true;
}

//----------------------------------------------------------------------------------------
void DataRemapTable::SetConversionTableMaxBitCount(unsigned int aconversionTableToMaxBitCount, unsigned int aconversionTableFromMaxBitCount)
{
	conversionTableToMaxBitCount = aconversionTableToMaxBitCount;
	conversionTableFromMaxBitCount = aconversionTableFromMaxBitCount;
}

//----------------------------------------------------------------------------------------
//Data mapping functions
//----------------------------------------------------------------------------------------
bool DataRemapTable::SetDataMapping(const std::wstring& mappingString, unsigned int sourceBitCount)
{
	//Tokenize the mapping string, and convert it to a set of mapping elements.
	std::list<MappingElement> mappingElements;
	bool parsingComplete = false;
	bool parsingResult = true;
	size_t nextChar = 0;
	while(!parsingComplete)
	{
		//Check if we've reached the end of the mapping string
		if(nextChar >= mappingString.size())
		{
			parsingComplete = true;
			parsingResult = true;
			continue;
		}

		//Process the next character in the mapping string
		switch(mappingString[nextChar])
		{
		//Process a bit forced to 0
		case L'0':{
			MappingElement element;
			element.forcedBitElement = true;
			element.forcedBitValue = false;
			mappingElements.push_back(element);
			++nextChar;
			break;}
		//Process a bit forced to 1
		case L'1':{
			MappingElement element;
			element.forcedBitElement = true;
			element.forcedBitValue = true;
			mappingElements.push_back(element);
			++nextChar;
			break;}

		//Process a bit mapping
		case L'[':{
			//Find the closing bracket
			size_t endBracketLocation = mappingString.find_first_of(L']', nextChar);

			//If we couldn't find a closing bracket, abort.
			if(endBracketLocation == std::wstring::npos)
			{
				parsingResult = false;
				parsingComplete = true;
				break;
			}

			//Extract the string representing the bit number from between the brackets
			++nextChar;
			std::wstring bitNumberString;
			bitNumberString = mappingString.substr(nextChar, endBracketLocation - nextChar);

			//Convert the bit number from a string to a numeric value
			unsigned int bitNumber = sourceBitCount;
			StringToInt(bitNumberString, bitNumber);

			//Confirm that the specified source bit number is valid
			if(bitNumber >= sourceBitCount)
			{
				parsingResult = false;
				parsingComplete = true;
				break;
			}

			//Save the mapping info
			MappingElement element;
			element.forcedBitElement = false;
			element.sourceDataBitNumber = bitNumber;
			mappingElements.push_back(element);

			//Set the next character as the character after the closing bracket
			nextChar = endBracketLocation + 1;
			break;}

		//Filter out whitespace
		case L' ':
		case L'\t':
			++nextChar;
			break;
		//If we encountered an invalid character, abort.
		default:
			parsingResult = false;
			parsingComplete = true;
			break;
		}
	}

	//If the mapping string parsing failed, return false.
	if(!parsingResult)
	{
		return false;
	}

	//Initialize our mapping settings
	bitMaskOriginal = 0;
	bitMaskConverted = 0;
	bitCountOriginal = sourceBitCount;
	bitCountConverted = 0;
	insertBottomBitCount = 0;
	insertTopBitCount = 0;
	discardBottomBitCount = 0;
	discardTopBitCount = 0;
	forcedSetBitMaskInConverted = 0;

	//Process our mapping elements, and build our mapping settings.
	unsigned int highestSourceBitNumberUsed = 0;
	unsigned int lowestSourceBitNumberUsed = sourceBitCount;
	bool allSourceBitsInRelativeOrder = true;
	bool foundFirstSourceMapping = false;
	int firstSourceMappingDisplacement;
	for(std::list<MappingElement>::const_reverse_iterator i = mappingElements.rbegin(); i != mappingElements.rend(); ++i)
	{
		//Check if this element is forcing this bit to a particular value, or specifying a
		//source bit to map.
		if(i->forcedBitElement)
		{
			//Set the value in the forced bit mask
			forcedSetBitMaskInConverted |= i->forcedBitValue? (1 << bitCountConverted): 0;

			//If we haven't encountered a source bit mapping yet, record this as an
			//inserted bit at the bottom of the converted data, otherwise record it as an
			//inserted bit at the top of the converted data.
			if(!foundFirstSourceMapping)
			{
				++insertBottomBitCount;
			}
			else
			{
				++insertTopBitCount;
			}
		}
		else
		{
			//Calculate the relative displacement between the source and target bit
			//numbers used for this mapping.
			int sourceMappingDisplacement = (int)i->sourceDataBitNumber - (int)bitCountConverted;

			//Since we've encountered a source mapping for the highest bit we've processed
			//so far, reset the insertTopBitCount value to 0.
			insertTopBitCount = 0;

			//If this is the first source bit mapping we've encountered, record the
			//displacement, otherwise compare it with the previously recorded value.
			if(!foundFirstSourceMapping)
			{
				foundFirstSourceMapping = true;
				firstSourceMappingDisplacement = sourceMappingDisplacement;
			}
			else
			{
				//If the displacement of the source and target bit numbers in this mapping
				//is different to the first displacement we recorded, the relative order
				//of all source bits is not maintained by this mapping. We record this
				//information to help us choose an appropriate conversion method below.
				if(sourceMappingDisplacement != firstSourceMappingDisplacement)
				{
					allSourceBitsInRelativeOrder = false;
				}
			}

			//Build our bit mapping entry
			BitMapping bitMappingEntry;
			bitMappingEntry.bitMaskOriginal = 1 << i->sourceDataBitNumber;
			bitMappingEntry.bitMaskConverted = 1 << bitCountConverted;
			dataBitMappings.push_back(bitMappingEntry);
			dataBitMappingsSize = (unsigned int)dataBitMappings.size();

			//Add the mapped bit to the source and target bit masks
			bitMaskOriginal |= 1 << i->sourceDataBitNumber;
			bitMaskConverted |= 1 << bitCountConverted;

			//Update the highest and lowest used source bit numbers
			if(i->sourceDataBitNumber > highestSourceBitNumberUsed)
			{
				highestSourceBitNumberUsed = i->sourceDataBitNumber;
			}
			if(i->sourceDataBitNumber < lowestSourceBitNumberUsed)
			{
				lowestSourceBitNumberUsed = i->sourceDataBitNumber;
			}
		}

		//Update the bitcount for the number of bits in our converted value
		++bitCountConverted;
	}

	//Calculate the number of bits discarded at each end of the original data during a
	//conversion.
	discardBottomBitCount = lowestSourceBitNumberUsed;
	discardTopBitCount = (sourceBitCount - 1) - highestSourceBitNumberUsed;

	//If the relative order of all used source bits is maintained by the mapping, flag
	//that we can use the shift and mask method to perform the conversion.
	useMethodShiftAndMask = allSourceBitsInRelativeOrder;

	//Determine whether to use physical conversion tables
	if(!conversionTableStateSetManually)
	{
		//Only build conversion tables if we can't use the shift and mask method. The
		//shift and mask method is so efficient, the table method is almost certainly
		//going to be slower due to the latency involved in memory access, and the high
		//likelihood of a cache miss with a conversion table.
		if(!useMethodShiftAndMask)
		{
			useMethodConversionTableTo = ((bitCountOriginal - (discardBottomBitCount + discardTopBitCount)) <= conversionTableToMaxBitCount);
			useMethodConversionTableFrom = ((bitCountConverted - (insertBottomBitCount + insertTopBitCount)) <= conversionTableFromMaxBitCount);
		}
		else
		{
			useMethodConversionTableTo = false;
			useMethodConversionTableFrom = false;
		}
	}

	//Build the physical conversion tables
	if(useMethodConversionTableTo)
	{
		useMethodConversionTableTo = false;
		unsigned int conversionTableToSize = (1 << (bitCountOriginal - (discardBottomBitCount + discardTopBitCount)));
		conversionTableTo.resize(conversionTableToSize, 0);
		unsigned int nextNumber = 0;
		for(unsigned int i = 0; i < conversionTableToSize; ++i)
		{
			conversionTableTo[i] = ConvertTo(nextNumber);
			nextNumber += (1 << discardBottomBitCount);
		}
		useMethodConversionTableTo = true;
	}
	if(useMethodConversionTableFrom)
	{
		useMethodConversionTableFrom = false;
		unsigned int conversionTableFromSize = (1 << (bitCountConverted - (insertBottomBitCount + insertTopBitCount)));
		conversionTableFrom.resize(conversionTableFromSize, 0);
		unsigned int nextNumber = 0;
		for(unsigned int i = 0; i < conversionTableFromSize; ++i)
		{
			conversionTableFrom[i] = ConvertFrom(nextNumber);
			nextNumber += (1 << insertBottomBitCount);
		}
		useMethodConversionTableFrom = true;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Data conversion functions
//----------------------------------------------------------------------------------------
unsigned int DataRemapTable::ConvertTo(unsigned int sourceData) const
{
	unsigned int result = 0;

	if(useMethodConversionTableTo)
	{
		result = conversionTableTo[(sourceData & bitMaskOriginal) >> discardBottomBitCount];
	}
	else if(useMethodShiftAndMask)
	{
		result = ((sourceData & bitMaskOriginal) >> discardBottomBitCount) << insertBottomBitCount;
		result |= forcedSetBitMaskInConverted;
	}
	else
	{
		for(unsigned int i = 0; i < dataBitMappingsSize; ++i)
		{
			result |= ((sourceData & dataBitMappings[i].bitMaskOriginal) != 0)? dataBitMappings[i].bitMaskConverted: 0;
		}
		result |= forcedSetBitMaskInConverted;
	}

	return result;
}

//----------------------------------------------------------------------------------------
unsigned int DataRemapTable::ConvertFrom(unsigned int sourceData) const
{
	unsigned int result = 0;

	if(useMethodConversionTableFrom)
	{
		result = conversionTableFrom[(sourceData & bitMaskConverted) >> insertBottomBitCount];
	}
	else if(useMethodShiftAndMask)
	{
		result = ((sourceData >> insertBottomBitCount) << discardBottomBitCount) & bitMaskOriginal;
	}
	else
	{
		for(unsigned int i = 0; i < dataBitMappingsSize; ++i)
		{
			result |= ((sourceData & dataBitMappings[i].bitMaskConverted) != 0)? dataBitMappings[i].bitMaskOriginal: 0;
		}
	}

	return result;
}
