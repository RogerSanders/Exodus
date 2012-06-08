//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Data::Data(unsigned int abitCount)
:data(0)
{
	Resize(abitCount);
}

//----------------------------------------------------------------------------------------
Data::Data(unsigned int abitCount, unsigned int adata)
:data(adata)
{
	Resize(abitCount);
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void Data::Resize(unsigned int abitCount)
{
	bitCount = (unsigned char)abitCount;
	//This somewhat strange mask generation code allows us to construct a mask for a
	//32-bit datatype on a 32-bit system. The simple method of shifting up 32 places and
	//subtracting 1 would fail in this case, since on x86 the shift count is modulus 32,
	//or in other words 1 << 32 = 1. The result of this operation, as well as the result
	//of a right shift on a signed type, is undefined according to the C++ standard.
	bitMask = (((1 << (bitCount - 1)) - 1) << 1) | 0x01;
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::MaskData()
{
	data &= GetBitMask();
}

//----------------------------------------------------------------------------------------
//Container size properties
//----------------------------------------------------------------------------------------
unsigned int Data::GetBitMask() const
{
	return bitMask;
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetMaxValue() const
{
	return GetBitMask();
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetByteSize() const
{
	return (bitCount + (bitsPerByte - 1)) / bitsPerByte;
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetHexCharCount() const
{
	return (bitCount + 3) / 4;
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetBitCount() const
{
	return bitCount;
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
	data = target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator+=(unsigned int target)
{
	data += target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator-=(unsigned int target)
{
	data -= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator*=(unsigned int target)
{
	data *= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator/=(unsigned int target)
{
	data /= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator&=(unsigned int target)
{
	data &= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator|=(unsigned int target)
{
	data |= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator^=(unsigned int target)
{
	data ^= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator%=(unsigned int target)
{
	data %= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator<<=(unsigned int target)
{
	data <<= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator>>=(unsigned int target)
{
	data >>= target;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
bool Data::operator==(unsigned int target) const
{
	return data == target;
}

//----------------------------------------------------------------------------------------
bool Data::operator!=(unsigned int target) const
{
	return data != target;
}

//----------------------------------------------------------------------------------------
bool Data::operator>(unsigned int target) const
{
	return data > target;
}

//----------------------------------------------------------------------------------------
bool Data::operator<(unsigned int target) const
{
	return data < target;
}

//----------------------------------------------------------------------------------------
bool Data::operator>=(unsigned int target) const
{
	return data >= target;
}

//----------------------------------------------------------------------------------------
bool Data::operator<=(unsigned int target) const
{
	return data <= target;
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
	data = target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator+=(const Data& target)
{
	data += target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator-=(const Data& target)
{
	data -= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator*=(const Data& target)
{
	data *= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator/=(const Data& target)
{
	data /= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator&=(const Data& target)
{
	data &= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator|=(const Data& target)
{
	data |= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator^=(const Data& target)
{
	data ^= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator%=(const Data& target)
{
	data %= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator<<=(const Data& target)
{
	data <<= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator>>=(const Data& target)
{
	data >>= target.data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
bool Data::operator==(const Data& target) const
{
	return data == target.data;
}

//----------------------------------------------------------------------------------------
bool Data::operator!=(const Data& target) const
{
	return data != target.data;
}

//----------------------------------------------------------------------------------------
bool Data::operator>(const Data& target) const
{
	return data > target.data;
}

//----------------------------------------------------------------------------------------
bool Data::operator<(const Data& target) const
{
	return data < target.data;
}

//----------------------------------------------------------------------------------------
bool Data::operator>=(const Data& target) const
{
	return data >= target.data;
}

//----------------------------------------------------------------------------------------
bool Data::operator<=(const Data& target) const
{
	return data <= target.data;
}

//----------------------------------------------------------------------------------------
//Unary operators
//----------------------------------------------------------------------------------------
Data Data::operator~() const
{
	Data temp(*this);
	temp.data = ~temp.data;
	temp.MaskData();
	return temp;
}

//----------------------------------------------------------------------------------------
Data& Data::operator++()
{
	++data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data& Data::operator--()
{
	--data;
	MaskData();
	return *this;
}

//----------------------------------------------------------------------------------------
Data Data::operator++(int)
{
	Data temp(*this);
	++data;
	MaskData();
	return temp;
}

//----------------------------------------------------------------------------------------
Data Data::operator--(int)
{
	Data temp(*this);
	--data;
	MaskData();
	return temp;
}

//----------------------------------------------------------------------------------------
//Data conversion
//----------------------------------------------------------------------------------------
Data Data::Convert(unsigned int abitCount) const
{
	return Data(abitCount, GetData());
}

//----------------------------------------------------------------------------------------
Data Data::SignExtend(unsigned int abitCount) const
{
	Data result(abitCount, GetData());
	result |= (!MSB())? 0: (~0 ^ GetBitMask());
	return result;
}

//----------------------------------------------------------------------------------------
//Data segment extraction/insertion
//----------------------------------------------------------------------------------------
unsigned int Data::GetData() const
{
	return data;
}

//----------------------------------------------------------------------------------------
Data& Data::SetData(unsigned int adata)
{
	data = adata;
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
	return (data & (1 << bitNumber)) != 0;
}

//----------------------------------------------------------------------------------------
Data& Data::SetBit(unsigned int bitNumber, bool state)
{
	data = ((data & ~(1 << bitNumber)) | ((unsigned int)state << bitNumber));
	return *this;
}

//----------------------------------------------------------------------------------------
unsigned char Data::GetByte(unsigned int byte) const
{
	return (unsigned char)GetDataSegment(byte * bitsPerByte, bitsPerByte);
}

//----------------------------------------------------------------------------------------
void Data::SetByte(unsigned int byte, unsigned int data)
{
	SetDataSegment(byte * bitsPerByte, bitsPerByte, data);
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetDataSegment(unsigned int bitStart, unsigned int abitCount) const
{
	return (data >> bitStart) & ((((1 << (abitCount - 1)) - 1) << 1) | 0x01);
}

//----------------------------------------------------------------------------------------
Data& Data::SetDataSegment(unsigned int bitStart, unsigned int abitCount, unsigned int adata)
{
	unsigned int tempMask = (((1 << (abitCount - 1)) - 1) << 1) | 0x01;
	data &= ~(tempMask << bitStart);
	data |= ((adata & tempMask) << bitStart);
	return *this;
}

//----------------------------------------------------------------------------------------
//Upper\Lower half functions
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
	target.data = GetUpperHalf();
	target.MaskData();
}

//----------------------------------------------------------------------------------------
void Data::GetLowerHalf(Data& target) const
{
	target.data = GetLowerHalf();
	target.MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetUpperHalf(unsigned int adata)
{
	unsigned int segmentBitCount = ((GetBitCount() + 1) / 2);
	SetUpperBits(segmentBitCount, adata);
}

//----------------------------------------------------------------------------------------
void Data::SetLowerHalf(unsigned int adata)
{
	unsigned int segmentBitCount = (GetBitCount() / 2);
	SetLowerBits(segmentBitCount, adata);
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
//Upper\Lower bit functions
//----------------------------------------------------------------------------------------
unsigned int Data::GetUpperBits(unsigned int abitCount) const
{
	return (GetData() >> (GetBitCount() - abitCount));
}

//----------------------------------------------------------------------------------------
unsigned int Data::GetLowerBits(unsigned int abitCount) const
{
	return (GetData() & (GetBitMask() >> (GetBitCount() - abitCount)));
}

//----------------------------------------------------------------------------------------
void Data::GetUpperBits(Data& target) const
{
	target.data = GetUpperBits(target.GetBitCount());
}

//----------------------------------------------------------------------------------------
void Data::GetLowerBits(Data& target) const
{
	target.data = GetLowerBits(target.GetBitCount());
}

//----------------------------------------------------------------------------------------
void Data::SetUpperBits(unsigned int abitCount, unsigned int adata)
{
	unsigned int targetMask = (((1 << (abitCount - 1)) - 1) << 1) | 0x01;
	data &= ~(targetMask << (GetBitCount() - abitCount));
	data |= ((adata & targetMask) << (GetBitCount() - abitCount));
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetLowerBits(unsigned int abitCount, unsigned int adata)
{
	unsigned int targetMask = (((1 << (abitCount - 1)) - 1) << 1) | 0x01;
	data &= ~targetMask;
	data |= (adata & targetMask);
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetUpperBits(const Data& target)
{
	data &= ~(target.GetBitMask() << (GetBitCount() - target.GetBitCount()));
	data |= (target.GetData() << (GetBitCount() - target.GetBitCount()));
	MaskData();
}

//----------------------------------------------------------------------------------------
void Data::SetLowerBits(const Data& target)
{
	data &= ~target.GetBitMask();
	data |= target.GetData();
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
	for(unsigned int i = 0; i < GetBitCount(); ++i)
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
	for(unsigned int i = 0; i < GetBitCount(); ++i)
	{
		if(GetBit(i))
		{
			++setBitCount;
		}
	}
	return setBitCount;
}

//----------------------------------------------------------------------------------------
//Stream operators
//----------------------------------------------------------------------------------------
Stream::ViewText& operator>>(Stream::ViewText& stream, Data& object)
{
	stream >> object.data;
	return stream;
}

//----------------------------------------------------------------------------------------
Stream::ViewText& operator<<(Stream::ViewText& stream, const Data& object)
{
	stream << object.data;
	return stream;
}

//----------------------------------------------------------------------------------------
Stream::ViewBinary& operator>>(Stream::ViewBinary& stream, Data& object)
{
	for(unsigned int i = 0; i < object.GetByteSize(); ++i)
	{
		unsigned char data;
		stream >> data;
		object.SetByte(i, data);
	}
	return stream;
}

//----------------------------------------------------------------------------------------
Stream::ViewBinary& operator<<(Stream::ViewBinary& stream, const Data& object)
{
	for(unsigned int i = 0; i < object.GetByteSize(); ++i)
	{
		stream << object.GetByte(i);
	}
	return stream;
}
