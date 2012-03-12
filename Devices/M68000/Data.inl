//namespace M68000 {
//
////---------------------------------------------------------------------------------------
////Constructors
////---------------------------------------------------------------------------------------
//Data::Data(Datatype atype)
//:type(atype), data(0)
//{}
//
//Data::Data(Datatype atype, unsigned int adata)
//:type(atype), data(adata)
//{}
//
////---------------------------------------------------------------------------------------
////Integer operators
////---------------------------------------------------------------------------------------
//Data Data::operator+(const unsigned int& target) const
//{
//	return Data(type, (data & type) + target);
//}
//
//Data Data::operator-(const unsigned int& target) const
//{
//	return Data(type, (data & type) - target);
//}
//
//Data Data::operator*(const unsigned int& target) const
//{
//	return Data(type, (data & type) * target);
//}
//
//Data Data::operator/(const unsigned int& target) const
//{
//	//##TODO## Evaluate and clean up this crap
//	Data temp(type);
//	temp.data = data & type;
//	if(Negative())
//	{
//		temp.data = ((0 - temp.data) & temp.type);
//	}
//
//	temp.data /= target;
//
//	if(Negative())
//	{
//		temp.data = (0 - temp.data) & temp.type;
//	}
//
//	return temp;
////	return Data(type, (data & type) / target);
//}
//
//Data Data::operator&(const unsigned int& target) const
//{
//	return Data(type, (data & type) & target);
//}
//
//Data Data::operator|(const unsigned int& target) const
//{
//	return Data(type, (data & type) | target);
//}
//
//Data Data::operator^(const unsigned int& target) const
//{
//	return Data(type, (data & type) ^ target);
//}
//
//Data Data::operator%(const unsigned int& target) const
//{
//	return Data(type, (data & type) % target);
//}
//
//Data Data::operator<<(const unsigned int& target) const
//{
//	return Data(type, (data & type) << target);
//}
//
//Data Data::operator>>(const unsigned int& target) const
//{
//	return Data(type, (data & type) >> target);
//}
//
//Data& Data::operator+=(const unsigned int& target)
//{
//	data += target;
//	return *this;
//}
//
//Data& Data::operator-=(const unsigned int& target)
//{
//	data -= target;
//	return *this;
//}
//
//Data& Data::operator*=(const unsigned int& target)
//{
//	data *= target;
//	return *this;
//}
//
//Data& Data::operator/=(const unsigned int& target)
//{
//	data /= target;
//	return *this;
//}
//
//
//Data& Data::operator&=(const unsigned int& target)
//{
//	data &= target;
//	return *this;
//}
//
//Data& Data::operator|=(const unsigned int& target)
//{
//	data |= target;
//	return *this;
//}
//
//Data& Data::operator^=(const unsigned int& target)
//{
//	data ^= target;
//	return *this;
//}
//
//Data& Data::operator%=(const unsigned int& target)
//{
//	data = (data & type) % target;
//	return *this;
//}
//
//Data& Data::operator<<=(const unsigned int& target)
//{
//	data <<= target;
//	return *this;
//}
//
//Data& Data::operator>>=(const unsigned int& target)
//{
//	data = (data & type) >> target;
//	return *this;
//}
//
//
//Data& Data::operator=(const unsigned int& target)
//{
//	data = target;
//	return *this;
//}
//
//bool Data::operator==(const unsigned int& target) const
//{
//	return (data & type) == (target & type);
//}
//
//bool Data::operator!=(const unsigned int& target) const
//{
//	return (data & type) != (target & type);
//}
//
////---------------------------------------------------------------------------------------
////Data operators
////---------------------------------------------------------------------------------------
//Data Data::operator+(const Data& target) const
//{
//	Data temp(*this);
//	temp.data += target.data;
//	temp.type = (temp.type > type)? type : temp.type;
//	return temp;
//}
//
//Data Data::operator-(const Data& target) const
//{
//	Data temp(*this);
//	temp.data -= target.data;
//	temp.type = (temp.type > type)? type : temp.type;
//	return temp;
//}
//
//Data Data::operator*(const Data& target) const
//{
//	Data temp(*this);
//	temp.data = (temp.data & temp.type) * (target.data & target.type);
//	temp.type = (temp.type > type)? type : temp.type;
//	return temp;
//}
//
//Data Data::operator/(const Data& target) const
//{
//	Data temp(*this);
//	temp.data = (temp.data & temp.type) / (target.data & target.type);
//	temp.type = (temp.type > type)? type: temp.type;
//	return temp;
//}
//
//Data Data::operator~() const
//{
//	Data temp(*this);
//	temp.data = ~temp.data;
//	return temp;
//}
//
//Data Data::operator&(const Data& target) const
//{
//	Data temp(*this);
//	temp.data &= target.data;
//	temp.type = (temp.type > type)? type : temp.type;
//	return temp;
//}
//
//Data Data::operator|(const Data& target) const
//{
//	Data temp(*this);
//	temp.data |= target.data;
//	temp.type = (temp.type > type)? type : temp.type;
//	return temp;
//}
//
//Data Data::operator^(const Data& target) const
//{
//	Data temp(*this);
//	temp.data ^= target.data;
//	temp.type = (temp.type > type)? type : temp.type;
//	return temp;
//}
//
//Data Data::operator%(const Data& target) const
//{
//	Data temp(*this);
//	temp.data = (temp.data & temp.type) % (target.data & target.type);
//	temp.type = (temp.type > type)? type : temp.type;
//	return temp;
//}
//
//Data Data::operator<<(const Data& target) const
//{
//	Data temp(*this);
//	temp.data <<= (target.data & target.type);
//	return temp;
//}
//
//Data Data::operator>>(const Data& target) const
//{
//	Data temp(*this);
//	temp.data &= temp.type;
////	temp.data >>= (target.data & target.type);
//	temp.data = (temp.data >> (target.data & target.type));
//	return temp;
//}
//
//Data& Data::operator++()
//{
//	++data;
//	return *this;
//}
//
//Data& Data::operator--()
//{
//	--data;
//	return *this;
//}
//
//Data Data::operator++(int)
//{
//	Data temp(*this);
//	++data;
//	return temp;
//}
//
//Data Data::operator--(int)
//{
//	Data temp(*this);
//	--data;
//	return temp;
//}
//
//Data& Data::operator+=(const Data& target)
//{
//	data += target.data;
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator-=(const Data& target)
//{
//	data -= target.data;
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator*=(const Data& target)
//{
//	data = (data & type) * (target.data & target.type);
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator/=(const Data& target)
//{
//	data = (data & type) / (target.data & target.type);
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator&=(const Data& target)
//{
//	data &= target.data;
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator|=(const Data& target)
//{
//	data |= target.data;
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator^=(const Data& target)
//{
//	data ^= target.data;
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator%=(const Data& target)
//{
//	data = (data & type) % (target.data & target.type);
//	type = (target.type > type)? type : target.type;
//	return *this;
//}
//
//Data& Data::operator<<=(const Data& target)
//{
//	data &= type;
//	data <<= (target.data & target.type);
//	return *this;
//}
//
//Data& Data::operator>>=(const Data& target)
//{
//	data &= type;
//	data >>= (target.data & target.type);
//	return *this;
//}
//
//Data& Data::operator=(const Data& target)
//{
//	data = target.data;
//	type = target.type;
//	return *this;
//}
//
//bool Data::operator==(const Data& target) const
//{
//	return (target.data & target.type) == (data & type);
//}
//
//bool Data::operator!=(const Data& target) const
//{
//	return (target.data & target.type) != (data & type);
//}
//
////---------------------------------------------------------------------------------------
////Native datatype conversion operators
////---------------------------------------------------------------------------------------
//Data::operator BYTE() const
//{
//	return BYTE(data & type);
//}
//
//Data::operator WORD() const
//{
//	return WORD(data & type);
//}
//
//Data::operator LONG() const
//{
//	return LONG(data & type);
//}
//
////---------------------------------------------------------------------------------------
////Data conversion
////---------------------------------------------------------------------------------------
//Data Data::Convert(Datatype size) const
//{
//	return Data(size, data & type);
//}
//
//Data Data::Combine(Data target)
//{
//	Data result(SIZE_LONG);
//	result.data = ((data & type & (type ^ target.type)) | (target.data & target.type));
//	result.type = (target.type > type)? type : target.type;
//	return result;
//}
//
//Data Data::SignExtend(Datatype size) const
//{
//	unsigned int upperBit = data & ((type >> 1) + 1);
//	Data result(size, (data & type));
//	result.data |= (upperBit == 0)? 0: size ^ type;
//	return result;
//}
//
//unsigned int Data::GetData() const
//{
//	return data & type;
//}
//
//unsigned int Data::GetDataSegment(int bitStart, int bitCount) const
//{
//	return (data >> bitStart) & ((1 << bitCount) - 1);
//}
//
//void Data::SetDataSegment(int bitStart, int bitCount, unsigned int adata)
//{
//	data &= ~(((1 << bitCount) - 1) << bitStart);
//	data |= ((adata & ((1 << bitCount) - 1)) << bitStart);
//}
//
//void Data::SetData(unsigned int adata)
//{
//	data = adata;
//}
//
//Datatype Data::GetType() const
//{
//	return type;
//}
//
//unsigned int Data::GetByteSize() const
//{
//	unsigned int size = 0;
//
//	switch(type)
//	{
//	case SIZE_BYTE:
//		size = 1;
//		break;
//	case SIZE_WORD:
//		size = 2;
//		break;
//	case SIZE_LONG:
//		size = 4;
//		break;
//	}
//
//	return size;
//}
//
//unsigned int Data::GetBitCount() const
//{
//	unsigned int bitCount = 0;
//
//	switch(type)
//	{
//	case SIZE_BYTE:
//		bitCount = 8;
//		break;
//	case SIZE_WORD:
//		bitCount = 16;
//		break;
//	case SIZE_LONG:
//		bitCount = 32;
//		break;
//	}
//
//	return bitCount;
//}
//
//bool Data::MSB() const
//{
//	return (data & (((unsigned int)type >> 1) + 1)) != 0;
//}
//
//bool Data::GetBit(unsigned int bitNumber) const
//{
//	return (data & (1 << bitNumber)) != 0;
//}
//
////---------------------------------------------------------------------------------------
////Result flags
////---------------------------------------------------------------------------------------
//bool Data::Zero() const
//{
//	return (data & type) == 0;
//}
//
////##FIX## Apparently, doing a bitshift on the type mask when it is an enum type does a sign-extend. We need to only perform operations on the mask when it is converted to an unsigned int.
//bool Data::Negative() const
//{
//	return (data & (((unsigned int)type >> 1) + 1)) != 0;
//}
//
//BYTE::BYTE()
//:Data(SIZE_BYTE)
//{}
//
//BYTE::BYTE(unsigned int adata)
//:Data(SIZE_BYTE, adata)
//{}
//
//WORD::WORD()
//:Data(SIZE_WORD)
//{}
//
//WORD::WORD(unsigned int adata)
//:Data(SIZE_WORD, adata)
//{}
//
//LONG::LONG()
//:Data(SIZE_LONG)
//{}
//
//LONG::LONG(unsigned int adata)
//:Data(SIZE_LONG, adata)
//{}
//
//} //Close namespace M68000
