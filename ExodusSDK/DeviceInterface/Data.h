#ifndef __DATA_H__
#define __DATA_H__
#include "StreamInterface/StreamInterface.pkg"

class Data
{
public:
	// Constants
	static const unsigned int BitsPerByte = 8;

public:
	// Constructors
	explicit inline Data(unsigned int bitCount);
	inline Data(unsigned int bitCount, unsigned int data);
	inline Data(const Data& source) = default;

	// Management functions
	inline void Resize(unsigned int bitCount);

	// Container size properties
	inline unsigned int GetBitMask() const;
	inline unsigned int GetMaxValue() const;
	inline unsigned int GetByteSize() const;
	inline unsigned int GetHexCharCount() const;
	inline unsigned int GetBitCount() const;

	// Integer operators
	inline Data operator+(unsigned int target) const;
	inline Data operator-(unsigned int target) const;
	inline Data operator*(unsigned int target) const;
	inline Data operator/(unsigned int target) const;
	inline Data operator&(unsigned int target) const;
	inline Data operator|(unsigned int target) const;
	inline Data operator^(unsigned int target) const;
	inline Data operator%(unsigned int target) const;
	inline Data operator<<(unsigned int target) const;
	inline Data operator>>(unsigned int target) const;

	inline Data& operator=(unsigned int target);
	inline Data& operator+=(unsigned int target);
	inline Data& operator-=(unsigned int target);
	inline Data& operator*=(unsigned int target);
	inline Data& operator/=(unsigned int target);
	inline Data& operator&=(unsigned int target);
	inline Data& operator|=(unsigned int target);
	inline Data& operator^=(unsigned int target);
	inline Data& operator%=(unsigned int target);
	inline Data& operator<<=(unsigned int target);
	inline Data& operator>>=(unsigned int target);

	inline bool operator==(unsigned int target) const;
	inline bool operator!=(unsigned int target) const;
	inline bool operator>(unsigned int target) const;
	inline bool operator<(unsigned int target) const;
	inline bool operator>=(unsigned int target) const;
	inline bool operator<=(unsigned int target) const;

	// Data operators
	inline Data operator+(const Data& target) const;
	inline Data operator-(const Data& target) const;
	inline Data operator*(const Data& target) const;
	inline Data operator/(const Data& target) const;
	inline Data operator&(const Data& target) const;
	inline Data operator|(const Data& target) const;
	inline Data operator^(const Data& target) const;
	inline Data operator%(const Data& target) const;
	inline Data operator<<(const Data& target) const;
	inline Data operator>>(const Data& target) const;

	inline Data& operator=(const Data& target);
	inline Data& operator+=(const Data& target);
	inline Data& operator-=(const Data& target);
	inline Data& operator*=(const Data& target);
	inline Data& operator/=(const Data& target);
	inline Data& operator&=(const Data& target);
	inline Data& operator|=(const Data& target);
	inline Data& operator^=(const Data& target);
	inline Data& operator%=(const Data& target);
	inline Data& operator<<=(const Data& target);
	inline Data& operator>>=(const Data& target);

	inline bool operator==(const Data& target) const;
	inline bool operator!=(const Data& target) const;
	inline bool operator>(const Data& target) const;
	inline bool operator<(const Data& target) const;
	inline bool operator>=(const Data& target) const;
	inline bool operator<=(const Data& target) const;

	// Unary operators
	inline Data operator~() const;
	inline Data& operator++();
	inline Data& operator--();
	inline Data operator++(int);
	inline Data operator--(int);

	// Data conversion
	inline Data Convert(unsigned int bitCount) const;
	inline Data SignExtend(unsigned int bitCount) const;

	// Data segment extraction/insertion
	inline unsigned int GetData() const;
	inline Data& SetData(unsigned int data);
	inline bool MSB() const;
	inline bool LSB() const;
	inline void MSB(bool state);
	inline void LSB(bool state);
	inline bool GetBit(unsigned int bitNumber) const;
	inline Data& SetBit(unsigned int bitNumber, bool state);
	inline unsigned char GetByteFromBottomUp(unsigned int byte) const;
	inline void SetByteFromBottomUp(unsigned int byte, unsigned int data);
	inline unsigned char GetByteFromTopDown(unsigned int byte) const;
	inline void SetByteFromTopDown(unsigned int byte, unsigned int data);
	inline unsigned int GetDataSegment(unsigned int bitStart, unsigned int bitCount) const;
	inline Data& SetDataSegment(unsigned int bitStart, unsigned int bitCount, unsigned int data);

	// Upper/Lower half functions
	inline unsigned int GetUpperHalf() const;
	inline unsigned int GetLowerHalf() const;
	inline void GetUpperHalf(Data& target) const;
	inline void GetLowerHalf(Data& target) const;
	inline void SetUpperHalf(unsigned int data);
	inline void SetLowerHalf(unsigned int data);
	inline void SetUpperHalf(const Data& target);
	inline void SetLowerHalf(const Data& target);

	// Upper/Lower bit functions
	inline unsigned int GetUpperBits(unsigned int bitCount) const;
	inline unsigned int GetLowerBits(unsigned int bitCount) const;
	inline void GetUpperBits(Data& target) const;
	inline void GetLowerBits(Data& target) const;
	inline void SetUpperBits(unsigned int bitCount, unsigned int data);
	inline void SetLowerBits(unsigned int bitCount, unsigned int data);
	inline void SetUpperBits(const Data& target);
	inline void SetLowerBits(const Data& target);

	// Data properties
	inline bool Even() const;
	inline bool Odd() const;
	inline bool Zero() const;
	inline bool NonZero() const;
	inline bool Negative() const;
	inline bool Positive() const;
	inline bool ParityEven() const;
	inline bool ParityOdd() const;
	inline unsigned int GetSetBitCount() const;
	inline bool GetHighestSetBitNumber(unsigned int& bitNumber) const;
	inline bool GetHighestSetBitMask(unsigned int& bitMask) const;
	inline bool GetLowestSetBitNumber(unsigned int& bitNumber) const;
	inline bool GetLowestSetBitMask(unsigned int& bitMask) const;

	// Stream operators
	friend inline Stream::ViewText& operator>>(Stream::ViewText& stream, Data& object);
	friend inline Stream::ViewText& operator<<(Stream::ViewText& stream, const Data& object);
	friend inline Stream::ViewBinary& operator>>(Stream::ViewBinary& stream, Data& object);
	friend inline Stream::ViewBinary& operator<<(Stream::ViewBinary& stream, const Data& object);

//##TODO## Eliminate all inheritance from this type, and mark this as private.
protected:
	// Management functions
	inline void MaskData();

protected:
	//##TODO## Evaluate if there is any way we can make this structure support 64-bit
	// types without hurting performance for 32-bit values. At the very least, we should be
	// able to come up with a way for our platform to support 64-bit register widths for
	// processors without breaking compatibility with existing devices, or requiring 64-bit
	// wide registers on the emulating system.
	unsigned int _data;
	unsigned int _bitMask;
	unsigned char _bitCount;
};

#include "Data.inl"
#endif
