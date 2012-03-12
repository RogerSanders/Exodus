#ifndef __Z80_DATA_H__
#define __Z80_DATA_H__
namespace Z80 {

enum Bitcount
{
	BITCOUNT_BYTE = 8,
	BITCOUNT_WORD = 16
};

class Z80Byte :public Data
{
public:
	//Constructors
	inline Z80Byte()
	:Data(BITCOUNT_BYTE)
	{}
	inline Z80Byte(unsigned int adata)
	:Data(BITCOUNT_BYTE, adata)
	{}
	explicit inline Z80Byte(const Data& adata)
	:Data(BITCOUNT_BYTE, adata.GetData())
	{}

	//Integer operators
	Z80Byte operator+(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp += target;
		return temp;
	}
	Z80Byte operator-(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp -= target;
		return temp;
	}
	Z80Byte operator*(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp *= target;
		return temp;
	}
	Z80Byte operator/(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp /= target;
		return temp;
	}
	Z80Byte operator&(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp &= target;
		return temp;
	}
	Z80Byte operator|(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp |= target;
		return temp;
	}
	Z80Byte operator^(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp ^= target;
		return temp;
	}
	Z80Byte operator%(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp %= target;
		return temp;
	}
	Z80Byte operator<<(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp <<= target;
		return temp;
	}
	Z80Byte operator>>(unsigned int target) const
	{
		Z80Byte temp(*this);
		temp >>= target;
		return temp;
	}
	Z80Byte& operator=(unsigned int target)
	{
		data = target;
		MaskData();
		return *this;
	}
	Z80Byte& operator+=(unsigned int target)
	{
		data += target;
		MaskData();
		return *this;
	}
	Z80Byte& operator-=(unsigned int target)
	{
		data -= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator*=(unsigned int target)
	{
		data *= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator/=(unsigned int target)
	{
		data /= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator&=(unsigned int target)
	{
		data &= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator|=(unsigned int target)
	{
		data |= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator^=(unsigned int target)
	{
		data ^= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator%=(unsigned int target)
	{
		data %= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator<<=(unsigned int target)
	{
		data <<= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator>>=(unsigned int target)
	{
		data >>= target;
		MaskData();
		return *this;
	}
	bool operator==(unsigned int target) const
	{
		return data == target;
	}
	bool operator!=(unsigned int target) const
	{
		return data != target;
	}
	bool operator>(unsigned int target) const
	{
		return data > target;
	}
	bool operator<(unsigned int target) const
	{
		return data < target;
	}
	bool operator>=(unsigned int target) const
	{
		return data >= target;
	}
	bool operator<=(unsigned int target) const
	{
		return data <= target;
	}

	//Z80Byte operators
	Z80Byte operator+(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp += target;
		return temp;
	}
	Z80Byte operator-(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp -= target;
		return temp;
	}
	Z80Byte operator*(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp *= target;
		return temp;
	}
	Z80Byte operator/(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp /= target;
		return temp;
	}
	Z80Byte operator&(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp &= target;
		return temp;
	}
	Z80Byte operator|(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp |= target;
		return temp;
	}
	Z80Byte operator^(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp ^= target;
		return temp;
	}
	Z80Byte operator%(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp %= target;
		return temp;
	}
	Z80Byte operator<<(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp <<= target;
		return temp;
	}
	Z80Byte operator>>(const Z80Byte& target) const
	{
		Z80Byte temp(*this);
		temp >>= target;
		return temp;
	}
	Z80Byte& operator=(const Z80Byte& target)
	{
		data = target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator+=(const Z80Byte& target)
	{
		data += target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator-=(const Z80Byte& target)
	{
		data -= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator*=(const Z80Byte& target)
	{
		data *= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator/=(const Z80Byte& target)
	{
		data /= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator&=(const Z80Byte& target)
	{
		data &= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator|=(const Z80Byte& target)
	{
		data |= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator^=(const Z80Byte& target)
	{
		data ^= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator%=(const Z80Byte& target)
	{
		data %= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator<<=(const Z80Byte& target)
	{
		data <<= target.data;
		MaskData();
		return *this;
	}
	Z80Byte& operator>>=(const Z80Byte& target)
	{
		data >>= target.data;
		MaskData();
		return *this;
	}
	bool operator==(const Z80Byte& target) const
	{
		return data == target.data;
	}
	bool operator!=(const Z80Byte& target) const
	{
		return data != target.data;
	}
	bool operator>(const Z80Byte& target) const
	{
		return data > target.data;
	}
	bool operator<(const Z80Byte& target) const
	{
		return data < target.data;
	}
	bool operator>=(const Z80Byte& target) const
	{
		return data >= target.data;
	}
	bool operator<=(const Z80Byte& target) const
	{
		return data <= target.data;
	}

	//Unary operators
	Z80Byte operator~() const
	{
		Z80Byte temp(*this);
		temp.data = ~temp.data;
		temp.MaskData();
		return temp;
	}
	Z80Byte& operator++()
	{
		++data;
		MaskData();
		return *this;
	}
	Z80Byte& operator--()
	{
		--data;
		MaskData();
		return *this;
	}
	Z80Byte operator++(int)
	{
		Z80Byte temp(*this);
		++data;
		MaskData();
		return temp;
	}
	Z80Byte operator--(int)
	{
		Z80Byte temp(*this);
		--data;
		MaskData();
		return temp;
	}
};

class Z80Word :public Data
{
public:
	//Constructors
	inline Z80Word()
	:Data(BITCOUNT_WORD)
	{}
	inline Z80Word(unsigned int adata)
	:Data(BITCOUNT_WORD, adata)
	{}
	explicit inline Z80Word(const Data& adata)
	:Data(BITCOUNT_WORD, adata.GetData())
	{}

	//Integer operators
	Z80Word operator+(unsigned int target) const
	{
		Z80Word temp(*this);
		temp += target;
		return temp;
	}
	Z80Word operator-(unsigned int target) const
	{
		Z80Word temp(*this);
		temp -= target;
		return temp;
	}
	Z80Word operator*(unsigned int target) const
	{
		Z80Word temp(*this);
		temp *= target;
		return temp;
	}
	Z80Word operator/(unsigned int target) const
	{
		Z80Word temp(*this);
		temp /= target;
		return temp;
	}
	Z80Word operator&(unsigned int target) const
	{
		Z80Word temp(*this);
		temp &= target;
		return temp;
	}
	Z80Word operator|(unsigned int target) const
	{
		Z80Word temp(*this);
		temp |= target;
		return temp;
	}
	Z80Word operator^(unsigned int target) const
	{
		Z80Word temp(*this);
		temp ^= target;
		return temp;
	}
	Z80Word operator%(unsigned int target) const
	{
		Z80Word temp(*this);
		temp %= target;
		return temp;
	}
	Z80Word operator<<(unsigned int target) const
	{
		Z80Word temp(*this);
		temp <<= target;
		return temp;
	}
	Z80Word operator>>(unsigned int target) const
	{
		Z80Word temp(*this);
		temp >>= target;
		return temp;
	}
	Z80Word& operator=(unsigned int target)
	{
		data = target;
		MaskData();
		return *this;
	}
	Z80Word& operator+=(unsigned int target)
	{
		data += target;
		MaskData();
		return *this;
	}
	Z80Word& operator-=(unsigned int target)
	{
		data -= target;
		MaskData();
		return *this;
	}
	Z80Word& operator*=(unsigned int target)
	{
		data *= target;
		MaskData();
		return *this;
	}
	Z80Word& operator/=(unsigned int target)
	{
		data /= target;
		MaskData();
		return *this;
	}
	Z80Word& operator&=(unsigned int target)
	{
		data &= target;
		MaskData();
		return *this;
	}
	Z80Word& operator|=(unsigned int target)
	{
		data |= target;
		MaskData();
		return *this;
	}
	Z80Word& operator^=(unsigned int target)
	{
		data ^= target;
		MaskData();
		return *this;
	}
	Z80Word& operator%=(unsigned int target)
	{
		data %= target;
		MaskData();
		return *this;
	}
	Z80Word& operator<<=(unsigned int target)
	{
		data <<= target;
		MaskData();
		return *this;
	}
	Z80Word& operator>>=(unsigned int target)
	{
		data >>= target;
		MaskData();
		return *this;
	}
	bool operator==(unsigned int target) const
	{
		return data == target;
	}
	bool operator!=(unsigned int target) const
	{
		return data != target;
	}
	bool operator>(unsigned int target) const
	{
		return data > target;
	}
	bool operator<(unsigned int target) const
	{
		return data < target;
	}
	bool operator>=(unsigned int target) const
	{
		return data >= target;
	}
	bool operator<=(unsigned int target) const
	{
		return data <= target;
	}

	//Z80Word operators
	Z80Word operator+(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp += target;
		return temp;
	}
	Z80Word operator-(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp -= target;
		return temp;
	}
	Z80Word operator*(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp *= target;
		return temp;
	}
	Z80Word operator/(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp /= target;
		return temp;
	}
	Z80Word operator&(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp &= target;
		return temp;
	}
	Z80Word operator|(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp |= target;
		return temp;
	}
	Z80Word operator^(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp ^= target;
		return temp;
	}
	Z80Word operator%(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp %= target;
		return temp;
	}
	Z80Word operator<<(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp <<= target;
		return temp;
	}
	Z80Word operator>>(const Z80Word& target) const
	{
		Z80Word temp(*this);
		temp >>= target;
		return temp;
	}
	Z80Word& operator=(const Z80Word& target)
	{
		data = target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator+=(const Z80Word& target)
	{
		data += target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator-=(const Z80Word& target)
	{
		data -= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator*=(const Z80Word& target)
	{
		data *= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator/=(const Z80Word& target)
	{
		data /= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator&=(const Z80Word& target)
	{
		data &= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator|=(const Z80Word& target)
	{
		data |= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator^=(const Z80Word& target)
	{
		data ^= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator%=(const Z80Word& target)
	{
		data %= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator<<=(const Z80Word& target)
	{
		data <<= target.data;
		MaskData();
		return *this;
	}
	Z80Word& operator>>=(const Z80Word& target)
	{
		data >>= target.data;
		MaskData();
		return *this;
	}
	bool operator==(const Z80Word& target) const
	{
		return data == target.data;
	}
	bool operator!=(const Z80Word& target) const
	{
		return data != target.data;
	}
	bool operator>(const Z80Word& target) const
	{
		return data > target.data;
	}
	bool operator<(const Z80Word& target) const
	{
		return data < target.data;
	}
	bool operator>=(const Z80Word& target) const
	{
		return data >= target.data;
	}
	bool operator<=(const Z80Word& target) const
	{
		return data <= target.data;
	}

	//Unary operators
	Z80Word operator~() const
	{
		Z80Word temp(*this);
		temp.data = ~temp.data;
		temp.MaskData();
		return temp;
	}
	Z80Word& operator++()
	{
		++data;
		MaskData();
		return *this;
	}
	Z80Word& operator--()
	{
		--data;
		MaskData();
		return *this;
	}
	Z80Word operator++(int)
	{
		Z80Word temp(*this);
		++data;
		MaskData();
		return temp;
	}
	Z80Word operator--(int)
	{
		Z80Word temp(*this);
		--data;
		MaskData();
		return temp;
	}
};

} //Close namespace Z80
#endif
