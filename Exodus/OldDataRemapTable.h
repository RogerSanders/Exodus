//##TODO## Remove this old remap table. The new version is much more powerful, and gives
//better performance.
#ifndef __OLDDATAREMAPTABLE_H__
#define __OLDDATAREMAPTABLE_H__
#include <string>
#include <vector>

class OldDataRemapTable
{
public:
	bool Build(const std::wstring& bitMaskString, unsigned int sourceBitCount, bool specifyConversionTableState = false, bool auseConversionTableTo = true, bool auseConversionTableFrom = true);

	//Data conversion functions
	unsigned int ConvertTo(unsigned int sourceData) const;
	unsigned int ConvertFrom(unsigned int sourceData) const;
	unsigned int GetBitCountConverted() const;

private:
	unsigned int bitMask;	//Mask of the lines to preserve in the source
	unsigned int bitCountConverted;	//Count of the number of set bits in outputBitMask
	unsigned int bitCountOriginal;	//Count of the number of bits in the normal data source
	unsigned int discardBottomBitCount;	//Count of zero bits at the bottom of bitMask
	unsigned int discardTopBitCount;	//Count of zero bits at the top of bitMask

	//Conversion table data when converting to the modified value
	bool useConversionTableTo;	//Flag indicating whether a physical conversion table is used
	unsigned int conversionTableToSize;	//Size of the physical conversion table
	std::vector<unsigned int> conversionTableTo;	//The physical conversion table

	//Conversion table data when converting from the modified value
	bool useConversionTableFrom;
	unsigned int conversionTableFromSize;
	std::vector<unsigned int> conversionTableFrom;
};

#endif
