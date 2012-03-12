#ifndef __DATAREMAPTABLE_H__
#define __DATAREMAPTABLE_H__
#include <string>
#include <vector>

class DataRemapTable
{
public:
	//Constructors
	DataRemapTable();

	//Data conversion settings
	void SetConversionTableState(bool auseConversionTableTo, bool auseConversionTableFrom);
	void SetConversionTableMaxBitCount(unsigned int aconversionTableToMaxBitCount, unsigned int aconversionTableFromMaxBitCount);

	//Data mapping functions
	bool SetDataMapping(const std::wstring& mappingString, unsigned int sourceBitCount);

	//Data conversion functions
	unsigned int ConvertTo(unsigned int sourceData) const;
	unsigned int ConvertFrom(unsigned int sourceData) const;
	unsigned int GetBitCountConverted() const;
	unsigned int GetBitMaskOriginalLinesPreserved() const;

private:
	//Structures
	struct BitMapping;
	struct MappingElement;

private:
	//Mapping settings
	unsigned int bitMaskOriginal;	//Mask of the lines to preserve in the original data
	unsigned int bitMaskConverted;  //Mask of the lines in the converted data that come from the original data
	unsigned int bitCountOriginal;	//Count of the number of bits in the original data
	unsigned int bitCountConverted;	//Count of the number of bits in the converted data
	unsigned int insertBottomBitCount;  //Count of new bits which are inserted at the bottom of the converted data
	unsigned int insertTopBitCount;     //Count of new bits which are inserted at the top of the converted data
	unsigned int discardBottomBitCount;	//Count of discarded bits at the bottom of the original data
	unsigned int discardTopBitCount;	//Count of discarded bits at the top of the original data
	unsigned int forcedSetBitMaskInConverted; //Mask of bits to force as set in the converted value

	//Conversion method settings
	bool useMethodConversionTableTo;
	bool useMethodConversionTableFrom;
	bool useMethodShiftAndMask;

	//Manual bit mapping data
	unsigned int dataBitMappingsSize; //We cache this purely as a paranoid optimization
	std::vector<BitMapping> dataBitMappings;

	//Conversion table data
	bool conversionTableStateSetManually;
	unsigned int conversionTableToMaxBitCount;
	unsigned int conversionTableFromMaxBitCount;
	std::vector<unsigned int> conversionTableTo;
	std::vector<unsigned int> conversionTableFrom;
};

#include "DataRemapTable.inl"
#endif
