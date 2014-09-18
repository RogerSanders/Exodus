#include "OldDataRemapTable.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
bool OldDataRemapTable::Build(const std::wstring& bitMaskString, unsigned int sourceBitCount, bool specifyConversionTableState, bool auseConversionTableTo, bool auseConversionTableFrom)
{
	//Convert bitMaskString to a binary value and load into bitMask
	StringToIntBase2(bitMaskString, bitMask);
	bitCountOriginal = sourceBitCount;

	//Build properties about the bitmask
	bitCountConverted = 0;
	discardBottomBitCount = 0;
	discardTopBitCount = 0;
	bool foundFirstSetBit = false;
	unsigned int bitMaskCopy = bitMask;
	for(unsigned int i = 0; i < sourceBitCount; ++i)
	{
		bool bit = (bitMaskCopy & 0x01) != 0;
		if(bit)
		{
			discardTopBitCount = 0;
			++bitCountConverted;
			foundFirstSetBit = true;
		}
		else
		{
			if(!foundFirstSetBit)
			{
				++discardBottomBitCount;
			}
			else
			{
				++discardTopBitCount;
			}
		}
		bitMaskCopy >>= 1;
	}

	//Determine whether to use physical conversion tables
	if(specifyConversionTableState)
	{
		useConversionTableFrom = auseConversionTableFrom;
		useConversionTableTo = auseConversionTableTo;
	}
	else
	{
		//##FIX## Take out these magic numbers. The caller should have some say in what
		//these limits should be. Provide a way for the caller to be able to specify
		//memory usage maximums, or conversion table bitcount limits, and use those saved
		//limits here. We can use these magic numbers as defaults for the limits.
		useConversionTableTo = ((bitCountOriginal - (discardBottomBitCount + discardTopBitCount)) <= 20);
		useConversionTableFrom = (bitCountConverted <= 20);
	}

	//Build the physical conversion tables
	if(useConversionTableTo)
	{
		useConversionTableTo = false;
		conversionTableToSize = (1 << (bitCountOriginal - (discardBottomBitCount + discardTopBitCount)));
		conversionTableTo.resize(conversionTableToSize, 0);
		unsigned int nextNumber = 0;
		for(unsigned int i = 0; i < conversionTableToSize; ++i)
		{
			conversionTableTo[i] = ConvertTo(nextNumber);
			nextNumber += (1 << discardBottomBitCount);
		}
		useConversionTableTo = true;
	}
	if(useConversionTableFrom)
	{
		useConversionTableFrom = false;
		conversionTableFromSize = (1 << bitCountConverted);
		conversionTableFrom.resize(conversionTableFromSize, 0);
		for(unsigned int i = 0; i < conversionTableFromSize; ++i)
		{
			conversionTableFrom[i] = ConvertFrom(i);
		}
		useConversionTableFrom = true;
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Data conversion functions
//----------------------------------------------------------------------------------------
unsigned int OldDataRemapTable::ConvertTo(unsigned int sourceData) const
{
	unsigned int result = 0;

	if(useConversionTableTo)
	{
		result = conversionTableTo[(sourceData & bitMask) >> discardBottomBitCount];
	}
	else
	{
		unsigned int currentOutputBitCount = 0;
		for(unsigned int i = discardBottomBitCount; i < (bitCountOriginal - discardTopBitCount); ++i)
		{
			if((bitMask & (1 << i)) != 0)
			{
				result |= (((sourceData >> i) & 0x01) << currentOutputBitCount);
				++currentOutputBitCount;
			}
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
unsigned int OldDataRemapTable::ConvertFrom(unsigned int sourceData) const
{
	unsigned int result = 0;

	if(useConversionTableFrom)
	{
		result = conversionTableFrom[sourceData];
	}
	else
	{
		unsigned int currentInputBitCount = 0;
		for(unsigned int i = discardBottomBitCount; i < (bitCountOriginal - discardTopBitCount); ++i)
		{
			if((bitMask & (1 << i)) != 0)
			{
				result |= (((sourceData >> currentInputBitCount) & 0x01) << i);
				++currentInputBitCount;
			}
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
unsigned int OldDataRemapTable::GetBitCountConverted() const
{
	return bitCountConverted;
}
