#include "DataRemapTable.h"
#include "DataConversion/DataConversion.pkg"
#include <list>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DataRemapTable::DataRemapTable()
:_conversionTableStateSetManually(false)
{
	//Set the default maximum bit counts for our conversion tables to 20 bits, which gives
	//us a maximum table size of 1 MegaByte.
	_conversionTableToMaxBitCount = 20;
	_conversionTableFromMaxBitCount = 20;
}

//----------------------------------------------------------------------------------------
//Data conversion settings
//----------------------------------------------------------------------------------------
void DataRemapTable::SetConversionTableState(bool useConversionTableTo, bool useConversionTableFrom)
{
	_useMethodConversionTableTo = useConversionTableTo;
	_useMethodConversionTableFrom = useConversionTableFrom;
	_conversionTableStateSetManually = true;
}

//----------------------------------------------------------------------------------------
void DataRemapTable::SetConversionTableMaxBitCount(unsigned int conversionTableToMaxBitCount, unsigned int conversionTableFromMaxBitCount)
{
	_conversionTableToMaxBitCount = conversionTableToMaxBitCount;
	_conversionTableFromMaxBitCount = conversionTableFromMaxBitCount;
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
	while (!parsingComplete)
	{
		//Check if we've reached the end of the mapping string
		if (nextChar >= mappingString.size())
		{
			parsingComplete = true;
			parsingResult = true;
			continue;
		}

		//Process the next character in the mapping string
		switch (mappingString[nextChar])
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
			if (endBracketLocation == std::wstring::npos)
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
			if (!StringToIntBase10(bitNumberString, bitNumber))
			{
				parsingComplete = true;
				parsingResult = true;
				continue;
			}

			//Confirm that the specified source bit number is valid
			if (bitNumber >= sourceBitCount)
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
	if (!parsingResult)
	{
		return false;
	}

	//Initialize our mapping settings
	_bitMaskOriginal = 0;
	_bitMaskConverted = 0;
	_bitCountOriginal = sourceBitCount;
	_bitCountConverted = 0;
	_insertBottomBitCount = 0;
	_insertTopBitCount = 0;
	_discardBottomBitCount = 0;
	_discardTopBitCount = 0;
	_forcedSetBitMaskInConverted = 0;

	//Process our mapping elements, and build our mapping settings.
	unsigned int highestSourceBitNumberUsed = 0;
	unsigned int lowestSourceBitNumberUsed = sourceBitCount;
	bool allSourceBitsInRelativeOrder = true;
	bool foundFirstSourceMapping = false;
	int firstSourceMappingDisplacement;
	for (std::list<MappingElement>::const_reverse_iterator i = mappingElements.rbegin(); i != mappingElements.rend(); ++i)
	{
		//Check if this element is forcing this bit to a particular value, or specifying a
		//source bit to map.
		if (i->forcedBitElement)
		{
			//Set the value in the forced bit mask
			_forcedSetBitMaskInConverted |= i->forcedBitValue? (1 << _bitCountConverted): 0;

			//If we haven't encountered a source bit mapping yet, record this as an
			//inserted bit at the bottom of the converted data, otherwise record it as an
			//inserted bit at the top of the converted data.
			if (!foundFirstSourceMapping)
			{
				++_insertBottomBitCount;
			}
			else
			{
				++_insertTopBitCount;
			}
		}
		else
		{
			//Calculate the relative displacement between the source and target bit
			//numbers used for this mapping.
			int sourceMappingDisplacement = (int)i->sourceDataBitNumber - (int)_bitCountConverted;

			//Since we've encountered a source mapping for the highest bit we've processed
			//so far, reset the insertTopBitCount value to 0.
			_insertTopBitCount = 0;

			//If this is the first source bit mapping we've encountered, record the
			//displacement, otherwise compare it with the previously recorded value.
			if (!foundFirstSourceMapping)
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
				if (sourceMappingDisplacement != firstSourceMappingDisplacement)
				{
					allSourceBitsInRelativeOrder = false;
				}
			}

			//Build our bit mapping entry
			BitMapping bitMappingEntry;
			bitMappingEntry.bitMaskOriginal = 1 << i->sourceDataBitNumber;
			bitMappingEntry.bitMaskConverted = 1 << _bitCountConverted;
			_dataBitMappings.push_back(bitMappingEntry);
			_dataBitMappingsSize = (unsigned int)_dataBitMappings.size();

			//Add the mapped bit to the source and target bit masks
			_bitMaskOriginal |= 1 << i->sourceDataBitNumber;
			_bitMaskConverted |= 1 << _bitCountConverted;

			//Update the highest and lowest used source bit numbers
			if (i->sourceDataBitNumber > highestSourceBitNumberUsed)
			{
				highestSourceBitNumberUsed = i->sourceDataBitNumber;
			}
			if (i->sourceDataBitNumber < lowestSourceBitNumberUsed)
			{
				lowestSourceBitNumberUsed = i->sourceDataBitNumber;
			}
		}

		//Update the bitcount for the number of bits in our converted value
		++_bitCountConverted;
	}

	//Calculate the number of bits discarded at each end of the original data during a
	//conversion.
	_discardBottomBitCount = lowestSourceBitNumberUsed;
	_discardTopBitCount = (sourceBitCount - 1) - highestSourceBitNumberUsed;

	//If the relative order of all used source bits is maintained by the mapping, flag
	//that we can use the shift and mask method to perform the conversion.
	_useMethodShiftAndMask = allSourceBitsInRelativeOrder;

	//Determine whether to use physical conversion tables
	if (!_conversionTableStateSetManually)
	{
		//Only build conversion tables if we can't use the shift and mask method. The
		//shift and mask method is so efficient, the table method is almost certainly
		//going to be slower due to the latency involved in memory access, and the high
		//likelihood of a cache miss with a conversion table.
		if (!_useMethodShiftAndMask)
		{
			_useMethodConversionTableTo = ((_bitCountOriginal - (_discardBottomBitCount + _discardTopBitCount)) <= _conversionTableToMaxBitCount);
			_useMethodConversionTableFrom = ((_bitCountConverted - (_insertBottomBitCount + _insertTopBitCount)) <= _conversionTableFromMaxBitCount);
		}
		else
		{
			_useMethodConversionTableTo = false;
			_useMethodConversionTableFrom = false;
		}
	}

	//Build the physical conversion tables
	if (_useMethodConversionTableTo)
	{
		_useMethodConversionTableTo = false;
		unsigned int conversionTableToSize = (1 << (_bitCountOriginal - (_discardBottomBitCount + _discardTopBitCount)));
		_conversionTableTo.resize(conversionTableToSize, 0);
		unsigned int nextNumber = 0;
		for (unsigned int i = 0; i < conversionTableToSize; ++i)
		{
			_conversionTableTo[i] = ConvertTo(nextNumber);
			nextNumber += (1 << _discardBottomBitCount);
		}
		_useMethodConversionTableTo = true;
	}
	if (_useMethodConversionTableFrom)
	{
		_useMethodConversionTableFrom = false;
		unsigned int conversionTableFromSize = (1 << (_bitCountConverted - (_insertBottomBitCount + _insertTopBitCount)));
		_conversionTableFrom.resize(conversionTableFromSize, 0);
		unsigned int nextNumber = 0;
		for (unsigned int i = 0; i < conversionTableFromSize; ++i)
		{
			_conversionTableFrom[i] = ConvertFrom(nextNumber);
			nextNumber += (1 << _insertBottomBitCount);
		}
		_useMethodConversionTableFrom = true;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Data conversion functions
//----------------------------------------------------------------------------------------
unsigned int DataRemapTable::ConvertTo(unsigned int sourceData) const
{
	unsigned int result = 0;

	if (_useMethodConversionTableTo)
	{
		result = _conversionTableTo[(sourceData & _bitMaskOriginal) >> _discardBottomBitCount];
	}
	else if (_useMethodShiftAndMask)
	{
		result = ((sourceData & _bitMaskOriginal) >> _discardBottomBitCount) << _insertBottomBitCount;
		result |= _forcedSetBitMaskInConverted;
	}
	else
	{
		for (unsigned int i = 0; i < _dataBitMappingsSize; ++i)
		{
			result |= ((sourceData & _dataBitMappings[i].bitMaskOriginal) != 0)? _dataBitMappings[i].bitMaskConverted: 0;
		}
		result |= _forcedSetBitMaskInConverted;
	}

	return result;
}

//----------------------------------------------------------------------------------------
unsigned int DataRemapTable::ConvertFrom(unsigned int sourceData) const
{
	unsigned int result = 0;

	if (_useMethodConversionTableFrom)
	{
		result = _conversionTableFrom[(sourceData & _bitMaskConverted) >> _insertBottomBitCount];
	}
	else if (_useMethodShiftAndMask)
	{
		result = ((sourceData >> _insertBottomBitCount) << _discardBottomBitCount) & _bitMaskOriginal;
	}
	else
	{
		for (unsigned int i = 0; i < _dataBitMappingsSize; ++i)
		{
			result |= ((sourceData & _dataBitMappings[i].bitMaskConverted) != 0)? _dataBitMappings[i].bitMaskOriginal: 0;
		}
	}

	return result;
}
