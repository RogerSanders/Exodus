//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Data::Data(unsigned int bitCount)
:_data(0)
{
	Resize(bitCount);
}

//----------------------------------------------------------------------------------------
Data::Data(unsigned int bitCount, unsigned int data)
:_data(data)
{
	Resize(bitCount);
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void Data::Resize(unsigned int bitCount)
{
	_bitCount = (unsigned char)bitCount;
	//This somewhat strange mask generation code allows us to construct a mask for a
	//32-bit datatype on a 32-bit system. The simple method of shifting up 32 places and
	//subtracting 1 would fail in this case, since on x86 the shift count is modulus 32,
	//or in other words 1 << 32 = 1. The result of this operation, as well as the result
	//of a right shift on a signed type, is undefined according to the C++ standard.
	_bitMask = (((1 << (_bitCount - 1)) - 1) << 1) | 0x01;
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::MaskData()
{
	_data &= GetBitMask();
}

//----------------------------------------------------------------------------------------
//Container size properties
//----------------------------------------------------------------------------------------
unsigned int Data::GetBitMask() const
{
	return _bitMask;
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetMaxValue() const
{
	return GetBitMask();
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetByteSize() const
{
	return (_bitCount + (BitsPerByte - 1)) / BitsPerByte;
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetHexCharCount() const
{
	return (_bitCount + 3) / 4;
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetBitCount() const
{
	return _bitCount;
}

//----------------------------------------------------------------------------------------
//Integer operators
//----------------------------------------------------------------------------------------
Data Data::operator+(unsigned int target) const
{
	Data temp(*this);
	temp += target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator-(unsigned int target) const
{
	Data temp(*this);
	temp -= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator*(unsigned int target) const
{
	Data temp(*this);
	temp *= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator/(unsigned int target) const
{
	Data temp(*this);
	temp /= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator&(unsigned int target) const
{
	Data temp(*this);
	temp &= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator|(unsigned int target) const
{
	Data temp(*this);
	temp |= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator^(unsigned int target) const
{
	Data temp(*this);
	temp ^= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator%(unsigned int target) const
{
	Data temp(*this);
	temp %= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator<<(unsigned int target) const
{
	Data temp(*this);
	temp <<= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator>>(unsigned int target) const
{
	Data temp(*this);
	temp >>= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data& Data::operator=(unsigned int target)
{
	_data = target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator+=(unsigned int target)
{
	_data += target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator-=(unsigned int target)
{
	_data -= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator*=(unsigned int target)
{
	_data *= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator/=(unsigned int target)
{
	_data /= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator&=(unsigned int target)
{
	_data &= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator|=(unsigned int target)
{
	_data |= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator^=(unsigned int target)
{
	_data ^= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator%=(unsigned int target)
{
	_data %= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator<<=(unsigned int target)
{
	_data <<= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator>>=(unsigned int target)
{
	_data >>= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
bool Data::operator==(unsigned int target) const
{
	return _data == target;
}

//----------------------------------------------------------------------------------------
bool Data::operator!=(unsigned int target) const
{
	return _data != target;
}

//----------------------------------------------------------------------------------------
bool Data::operator>(unsigned int target) const
{
	return _data > target;
}

//----------------------------------------------------------------------------------------
bool Data::operator<(unsigned int target) const
{
	return _data < target;
}

//----------------------------------------------------------------------------------------
bool Data::operator>=(unsigned int target) const
{
	return _data >= target;
}

//----------------------------------------------------------------------------------------
bool Data::operator<=(unsigned int target) const
{
	return _data <= target;
}

//----------------------------------------------------------------------------------------
//Data operators
//----------------------------------------------------------------------------------------
Data Data::operator+(const Data& target) const
{
	Data temp(*this);
	temp += target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator-(const Data& target) const
{
	Data temp(*this);
	temp -= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator*(const Data& target) const
{
	Data temp(*this);
	temp *= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator/(const Data& target) const
{
	Data temp(*this);
	temp /= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator&(const Data& target) const
{
	Data temp(*this);
	temp &= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator|(const Data& target) const
{
	Data temp(*this);
	temp |= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator^(const Data& target) const
{
	Data temp(*this);
	temp ^= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator%(const Data& target) const
{
	Data temp(*this);
	temp %= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator<<(const Data& target) const
{
	Data temp(*this);
	temp <<= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator>>(const Data& target) const
{
	Data temp(*this);
	temp >>= target;
	return temp;
}

//----------------------------------------------------------------------------------------
Data& Data::operator=(const Data& target)
{
	_data = target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator+=(const Data& target)
{
	_data += target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator-=(const Data& target)
{
	_data -= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator*=(const Data& target)
{
	_data *= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator/=(const Data& target)
{
	_data /= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator&=(const Data& target)
{
	_data &= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator|=(const Data& target)
{
	_data |= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator^=(const Data& target)
{
	_data ^= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator%=(const Data& target)
{
	_data %= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator<<=(const Data& target)
{
	_data <<= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator>>=(const Data& target)
{
	_data >>= target._data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
bool Data::operator==(const Data& target) const
{
	return _data == target._data;
}

//----------------------------------------------------------------------------------------
bool Data::operator!=(const Data& target) const
{
	return _data != target._data;
}

//----------------------------------------------------------------------------------------
bool Data::operator>(const Data& target) const
{
	return _data > target._data;
}

//----------------------------------------------------------------------------------------
bool Data::operator<(const Data& target) const
{
	return _data < target._data;
}

//----------------------------------------------------------------------------------------
bool Data::operator>=(const Data& target) const
{
	return _data >= target._data;
}

//----------------------------------------------------------------------------------------
bool Data::operator<=(const Data& target) const
{
	return _data <= target._data;
}

//----------------------------------------------------------------------------------------
//Unary operators
//----------------------------------------------------------------------------------------
Data Data::operator~() const
{
	Data temp(*this);
	temp._data = ~temp._data;
	temp.MaskData();
	return temp;
}

//----------------------------------------------------------------------------------------
Data& Data::operator++()
{
	++_data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator--()
{
	--_data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data Data::operator++(int)
{
	Data temp(*this);
	++_data;
	MaskData();
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator--(int)
{
	Data temp(*this);
	--_data;
	MaskData();
	return temp;
}

//----------------------------------------------------------------------------------------
//Data conversion
//----------------------------------------------------------------------------------------
Data Data::Convert(unsigned int bitCount) const
{
	return Data(bitCount, GetData());
}

//----------------------------------------------------------------------------------------
Data Data::SignExtend(unsigned int bitCount) const
{
	Data result(bitCount, GetData());
	result |= (!MSB())? 0: (~0 ^ GetBitMask());
	return result;
}

//----------------------------------------------------------------------------------------
//Data segment extraction/insertion
//----------------------------------------------------------------------------------------
unsigned int Data::GetData() const
{
	return _data;
}

//----------------------------------------------------------------------------------------
Data& Data::SetData(unsigned int data)
{
	_data = data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
bool Data::MSB() const
{
	return GetBit(GetBitCount() - 1);
}

//----------------------------------------------------------------------------------------
bool Data::LSB() const
{
	return GetBit(0);
}

//----------------------------------------------------------------------------------------
void Data::MSB(bool state)
{
	SetBit(GetBitCount() - 1, state);
}

//----------------------------------------------------------------------------------------
void Data::LSB(bool state)
{
	SetBit(0, state);
}

//----------------------------------------------------------------------------------------
bool Data::GetBit(unsigned int bitNumber) const
{
	return (_data & (1 << bitNumber)) != 0;
}

//----------------------------------------------------------------------------------------
Data& Data::SetBit(unsigned int bitNumber, bool state)
{
	_data = ((_data & ~(1 << bitNumber)) | ((unsigned int)state << bitNumber));
	return *this;
}

//----------------------------------------------------------------------------------------
unsigned char Data::GetByteFromBottomUp(unsigned int byte) const
{
	return (unsigned char)GetDataSegment(byte * BitsPerByte, BitsPerByte);
}

//----------------------------------------------------------------------------------------
void Data::SetByteFromBottomUp(unsigned int byte, unsigned int data)
{
	SetDataSegment(byte * BitsPerByte, BitsPerByte, data);
}

//----------------------------------------------------------------------------------------
unsigned char Data::GetByteFromTopDown(unsigned int byte) const
{
	return (unsigned char)GetDataSegment(((GetByteSize() - 1) - byte) * BitsPerByte, BitsPerByte);
}

//----------------------------------------------------------------------------------------
void Data::SetByteFromTopDown(unsigned int byte, unsigned int data)
{
	SetDataSegment(((GetByteSize() - 1) - byte) * BitsPerByte, BitsPerByte, data);
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetDataSegment(unsigned int bitStart, unsigned int bitCount) const
{
	return (_data >> bitStart) & ((((1 << (bitCount - 1)) - 1) << 1) | 0x01);
}

//----------------------------------------------------------------------------------------
Data& Data::SetDataSegment(unsigned int bitStart, unsigned int bitCount, unsigned int data)
{
	unsigned int tempMask = (((1 << (bitCount - 1)) - 1) << 1) | 0x01;
	_data &= ~(tempMask << bitStart);
	_data |= ((data & tempMask) << bitStart);
	return *this;
}

//----------------------------------------------------------------------------------------
//Upper/Lower half functions
//----------------------------------------------------------------------------------------
unsigned int Data::GetUpperHalf() const
{
	return (GetData() >> ((GetBitCount() + 1) / 2));
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetLowerHalf() const
{
	return (GetData() & (GetBitMask() >> (GetBitCount() / 2)));
}

//----------------------------------------------------------------------------------------
void Data::GetUpperHalf(Data& target) const
{
	target._data = GetUpperHalf();
	target.MaskData();
}

//----------------------------------------------------------------------------------------
void Data::GetLowerHalf(Data& target) const
{
	target._data = GetLowerHalf();
	target.MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetUpperHalf(unsigned int data)
{
	unsigned int segmentBitCount = ((GetBitCount() + 1) / 2);
	SetUpperBits(segmentBitCount, data);
}

//----------------------------------------------------------------------------------------
void Data::SetLowerHalf(unsigned int data)
{
	unsigned int segmentBitCount = (GetBitCount() / 2);
	SetLowerBits(segmentBitCount, data);
}

//----------------------------------------------------------------------------------------
void Data::SetUpperHalf(const Data& target)
{
	SetUpperHalf(target.GetData());
}

//----------------------------------------------------------------------------------------
void Data::SetLowerHalf(const Data& target)
{
	SetLowerHalf(target.GetData());
}

//----------------------------------------------------------------------------------------
//Upper/Lower bit functions
//----------------------------------------------------------------------------------------
unsigned int Data::GetUpperBits(unsigned int bitCount) const
{
	return (GetData() >> (GetBitCount() - bitCount));
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetLowerBits(unsigned int bitCount) const
{
	return (GetData() & (GetBitMask() >> (GetBitCount() - bitCount)));
}

//----------------------------------------------------------------------------------------
void Data::GetUpperBits(Data& target) const
{
	target._data = GetUpperBits(target.GetBitCount());
}

//----------------------------------------------------------------------------------------
void Data::GetLowerBits(Data& target) const
{
	target._data = GetLowerBits(target.GetBitCount());
}

//----------------------------------------------------------------------------------------
void Data::SetUpperBits(unsigned int bitCount, unsigned int data)
{
	unsigned int targetMask = (((1 << (bitCount - 1)) - 1) << 1) | 0x01;
	_data &= ~(targetMask << (GetBitCount() - bitCount));
	_data |= ((data & targetMask) << (GetBitCount() - bitCount));
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetLowerBits(unsigned int bitCount, unsigned int data)
{
	unsigned int targetMask = (((1 << (bitCount - 1)) - 1) << 1) | 0x01;
	_data &= ~targetMask;
	_data |= (data & targetMask);
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetUpperBits(const Data& target)
{
	_data &= ~(target.GetBitMask() << (GetBitCount() - target.GetBitCount()));
	_data |= (target.GetData() << (GetBitCount() - target.GetBitCount()));
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetLowerBits(const Data& target)
{
	_data &= ~target.GetBitMask();
	_data |= target.GetData();
	MaskData();
}

//----------------------------------------------------------------------------------------
//Data properties
//----------------------------------------------------------------------------------------
bool Data::Even() const
{
	return !LSB();
}

//----------------------------------------------------------------------------------------
bool Data::Odd() const
{
	return LSB();
}

//----------------------------------------------------------------------------------------
bool Data::Zero() const
{
	return GetData() == 0;
}

//----------------------------------------------------------------------------------------
bool Data::NonZero() const
{
	return GetData() != 0;
}

//----------------------------------------------------------------------------------------
bool Data::Negative() const
{
	return MSB();
}

//----------------------------------------------------------------------------------------
bool Data::Positive() const
{
	return !MSB();
}

//----------------------------------------------------------------------------------------
bool Data::ParityEven() const
{
	bool parity = true;
	for (unsigned int i = 0; i < GetBitCount(); ++i)
	{
		parity ^= (GetData() >> i) & 0x01;
	}
	return parity;
}

//----------------------------------------------------------------------------------------
bool Data::ParityOdd() const
{
	return !ParityEven();
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetSetBitCount() const
{
	unsigned int setBitCount = 0;
	for (unsigned int i = 0; i < GetBitCount(); ++i)
	{
		if (GetBit(i))
		{
			++setBitCount;
		}
	}
	return setBitCount;
}

//----------------------------------------------------------------------------------------
bool Data::GetHighestSetBitNumber(unsigned int& bitNumber) const
{
	//If the data is zero, return false, since there's no set bits in the data.
	if (_data == 0)
	{
		return false;
	}

	//Calculate the number of the highest set bit in the data
	unsigned int searchData = _data;
	bitNumber = 0;
	while ((searchData >>= 1) != 0)
	{
		++bitNumber;
	}

	//Since we managed to locate the highest set bit, return true.
	return true;
}

//----------------------------------------------------------------------------------------
bool Data::GetHighestSetBitMask(unsigned int& bitMask) const
{
	//If the data is zero, return false, since there's no set bits in the data.
	if (_data == 0)
	{
		return false;
	}

	//Calculate the mask of the highest set bit in the data
	unsigned int searchData = _data;
	bitMask = 1;
	while ((searchData >>= 1) != 0)
	{
		bitMask <<= 1;
	}

	//Since we managed to locate the highest set bit mask, return true.
	return true;
}

//----------------------------------------------------------------------------------------
bool Data::GetLowestSetBitNumber(unsigned int& bitNumber) const
{
	//If the data is zero, return false, since there's no set bits in the data.
	if (_data == 0)
	{
		return false;
	}

	//Calculate the number of the lowest set bit in the data
	bitNumber = 0;
	while ((_data & (1 << bitNumber)) == 0)
	{
		++bitNumber;
	}

	//Since we managed to locate the lowest set bit, return true.
	return true;
}

//----------------------------------------------------------------------------------------
bool Data::GetLowestSetBitMask(unsigned int& bitMask) const
{
	//If the data is zero, return false, since there's no set bits in the data.
	if (_data == 0)
	{
		return false;
	}

	//Calculate the mask of the lowest set bit in the data
	bitMask = 1;
	while ((_data & bitMask) == 0)
	{
		bitMask <<= 1;
	}

	//Since we managed to locate the lowest set bit mask, return true.
	return true;
}

//----------------------------------------------------------------------------------------
//Stream operators
//----------------------------------------------------------------------------------------
Stream::ViewText& operator>>(Stream::ViewText& stream, Data& object)
{
	stream >> object._data;
	return stream;
}

//----------------------------------------------------------------------------------------
Stream::ViewText& operator<<(Stream::ViewText& stream, const Data& object)
{
	stream << object._data;
	return stream;
}

//----------------------------------------------------------------------------------------
Stream::ViewBinary& operator>>(Stream::ViewBinary& stream, Data& object)
{
	for (unsigned int i = 0; i < object.GetByteSize(); ++i)
	{
		unsigned char data;
		stream >> data;
		object.SetByteFromTopDown(i, data);
	}
	return stream;
}

//----------------------------------------------------------------------------------------
Stream::ViewBinary& operator<<(Stream::ViewBinary& stream, const Data& object)
{
	for (unsigned int i = 0; i < object.GetByteSize(); ++i)
	{
		stream << object.GetByteFromTopDown(i);
	}
	return stream;
}
