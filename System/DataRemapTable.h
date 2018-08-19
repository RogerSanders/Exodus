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
	void SetConversionTableState(bool useConversionTableTo, bool useConversionTableFrom);
	void SetConversionTableMaxBitCount(unsigned int conversionTableToMaxBitCount, unsigned int conversionTableFromMaxBitCount);

	//Data mapping functions
	bool SetDataMapping(const std::wstring& mappingString, unsigned int sourceBitCount);

	//Data conversion functions
	unsigned int ConvertTo(unsigned int sourceData) const;
	unsigned int ConvertFrom(unsigned int sourceData) const;
	inline unsigned int GetBitCountConverted() const;
	inline unsigned int GetBitMaskOriginalLinesPreserved() const;

private:
	//Structures
	struct BitMapping;
	struct MappingElement;

private:
	//Mapping settings
	unsigned int _bitMaskOriginal;   //Mask of the lines to preserve in the original data
	unsigned int _bitMaskConverted;  //Mask of the lines in the converted data that come from the original data
	unsigned int _bitCountOriginal;  //Count of the number of bits in the original data
	unsigned int _bitCountConverted; //Count of the number of bits in the converted data
	unsigned int _insertBottomBitCount;  //Count of new bits which are inserted at the bottom of the converted data
	unsigned int _insertTopBitCount;     //Count of new bits which are inserted at the top of the converted data
	unsigned int _discardBottomBitCount; //Count of discarded bits at the bottom of the original data
	unsigned int _discardTopBitCount;    //Count of discarded bits at the top of the original data
	unsigned int _forcedSetBitMaskInConverted; //Mask of bits to force as set in the converted value

	//Conversion method settings
	bool _useMethodConversionTableTo;
	bool _useMethodConversionTableFrom;
	bool _useMethodShiftAndMask;

	//Manual bit mapping data
	unsigned int _dataBitMappingsSize; //We cache this purely as a paranoid optimization
	std::vector<BitMapping> _dataBitMappings;

	//Conversion table data
	bool _conversionTableStateSetManually;
	unsigned int _conversionTableToMaxBitCount;
	unsigned int _conversionTableFromMaxBitCount;
	std::vector<unsigned int> _conversionTableTo;
	std::vector<unsigned int> _conversionTableFrom;
};

#include "DataRemapTable.inl"
#endif
