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
	// Constructors
	inline Z80Byte()
	:Data(BITCOUNT_BYTE)
	{ }
	inline Z80Byte(unsigned int data)
	:Data(BITCOUNT_BYTE, data)
	{ }
	explicit inline Z80Byte(const Data& data)
	:Data(BITCOUNT_BYTE, data.GetData())
	{ }
	inline Z80Byte(const Z80Byte& source) = default;

	// Integer operators
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
		_data = target;
		MaskData();
		return *this;
	}
	Z80Byte& operator+=(unsigned int target)
	{
		_data += target;
		MaskData();
		return *this;
	}
	Z80Byte& operator-=(unsigned int target)
	{
		_data -= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator*=(unsigned int target)
	{
		_data *= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator/=(unsigned int target)
	{
		_data /= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator&=(unsigned int target)
	{
		_data &= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator|=(unsigned int target)
	{
		_data |= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator^=(unsigned int target)
	{
		_data ^= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator%=(unsigned int target)
	{
		_data %= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator<<=(unsigned int target)
	{
		_data <<= target;
		MaskData();
		return *this;
	}
	Z80Byte& operator>>=(unsigned int target)
	{
		_data >>= target;
		MaskData();
		return *this;
	}
	bool operator==(unsigned int target) const
	{
		return _data == target;
	}
	bool operator!=(unsigned int target) const
	{
		return _data != target;
	}
	bool operator>(unsigned int target) const
	{
		return _data > target;
	}
	bool operator<(unsigned int target) const
	{
		return _data < target;
	}
	bool operator>=(unsigned int target) const
	{
		return _data >= target;
	}
	bool operator<=(unsigned int target) const
	{
		return _data <= target;
	}

	// Z80Byte operators
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
		_data = target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator+=(const Z80Byte& target)
	{
		_data += target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator-=(const Z80Byte& target)
	{
		_data -= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator*=(const Z80Byte& target)
	{
		_data *= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator/=(const Z80Byte& target)
	{
		_data /= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator&=(const Z80Byte& target)
	{
		_data &= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator|=(const Z80Byte& target)
	{
		_data |= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator^=(const Z80Byte& target)
	{
		_data ^= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator%=(const Z80Byte& target)
	{
		_data %= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator<<=(const Z80Byte& target)
	{
		_data <<= target._data;
		MaskData();
		return *this;
	}
	Z80Byte& operator>>=(const Z80Byte& target)
	{
		_data >>= target._data;
		MaskData();
		return *this;
	}
	bool operator==(const Z80Byte& target) const
	{
		return _data == target._data;
	}
	bool operator!=(const Z80Byte& target) const
	{
		return _data != target._data;
	}
	bool operator>(const Z80Byte& target) const
	{
		return _data > target._data;
	}
	bool operator<(const Z80Byte& target) const
	{
		return _data < target._data;
	}
	bool operator>=(const Z80Byte& target) const
	{
		return _data >= target._data;
	}
	bool operator<=(const Z80Byte& target) const
	{
		return _data <= target._data;
	}

	// Unary operators
	Z80Byte operator~() const
	{
		Z80Byte temp(*this);
		temp._data = ~temp._data;
		temp.MaskData();
		return temp;
	}
	Z80Byte& operator++()
	{
		++_data;
		MaskData();
		return *this;
	}
	Z80Byte& operator--()
	{
		--_data;
		MaskData();
		return *this;
	}
	Z80Byte operator++(int)
	{
		Z80Byte temp(*this);
		++_data;
		MaskData();
		return temp;
	}
	Z80Byte operator--(int)
	{
		Z80Byte temp(*this);
		--_data;
		MaskData();
		return temp;
	}
};

class Z80Word :public Data
{
public:
	// Constructors
	inline Z80Word()
	:Data(BITCOUNT_WORD)
	{ }
	inline Z80Word(unsigned int data)
	:Data(BITCOUNT_WORD, data)
	{ }
	explicit inline Z80Word(const Data& data)
	:Data(BITCOUNT_WORD, data.GetData())
	{ }
	inline Z80Word(const Z80Word& source) = default;

	// Integer operators
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
		_data = target;
		MaskData();
		return *this;
	}
	Z80Word& operator+=(unsigned int target)
	{
		_data += target;
		MaskData();
		return *this;
	}
	Z80Word& operator-=(unsigned int target)
	{
		_data -= target;
		MaskData();
		return *this;
	}
	Z80Word& operator*=(unsigned int target)
	{
		_data *= target;
		MaskData();
		return *this;
	}
	Z80Word& operator/=(unsigned int target)
	{
		_data /= target;
		MaskData();
		return *this;
	}
	Z80Word& operator&=(unsigned int target)
	{
		_data &= target;
		MaskData();
		return *this;
	}
	Z80Word& operator|=(unsigned int target)
	{
		_data |= target;
		MaskData();
		return *this;
	}
	Z80Word& operator^=(unsigned int target)
	{
		_data ^= target;
		MaskData();
		return *this;
	}
	Z80Word& operator%=(unsigned int target)
	{
		_data %= target;
		MaskData();
		return *this;
	}
	Z80Word& operator<<=(unsigned int target)
	{
		_data <<= target;
		MaskData();
		return *this;
	}
	Z80Word& operator>>=(unsigned int target)
	{
		_data >>= target;
		MaskData();
		return *this;
	}
	bool operator==(unsigned int target) const
	{
		return _data == target;
	}
	bool operator!=(unsigned int target) const
	{
		return _data != target;
	}
	bool operator>(unsigned int target) const
	{
		return _data > target;
	}
	bool operator<(unsigned int target) const
	{
		return _data < target;
	}
	bool operator>=(unsigned int target) const
	{
		return _data >= target;
	}
	bool operator<=(unsigned int target) const
	{
		return _data <= target;
	}

	// Z80Word operators
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
		_data = target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator+=(const Z80Word& target)
	{
		_data += target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator-=(const Z80Word& target)
	{
		_data -= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator*=(const Z80Word& target)
	{
		_data *= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator/=(const Z80Word& target)
	{
		_data /= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator&=(const Z80Word& target)
	{
		_data &= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator|=(const Z80Word& target)
	{
		_data |= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator^=(const Z80Word& target)
	{
		_data ^= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator%=(const Z80Word& target)
	{
		_data %= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator<<=(const Z80Word& target)
	{
		_data <<= target._data;
		MaskData();
		return *this;
	}
	Z80Word& operator>>=(const Z80Word& target)
	{
		_data >>= target._data;
		MaskData();
		return *this;
	}
	bool operator==(const Z80Word& target) const
	{
		return _data == target._data;
	}
	bool operator!=(const Z80Word& target) const
	{
		return _data != target._data;
	}
	bool operator>(const Z80Word& target) const
	{
		return _data > target._data;
	}
	bool operator<(const Z80Word& target) const
	{
		return _data < target._data;
	}
	bool operator>=(const Z80Word& target) const
	{
		return _data >= target._data;
	}
	bool operator<=(const Z80Word& target) const
	{
		return _data <= target._data;
	}

	// Unary operators
	Z80Word operator~() const
	{
		Z80Word temp(*this);
		temp._data = ~temp._data;
		temp.MaskData();
		return temp;
	}
	Z80Word& operator++()
	{
		++_data;
		MaskData();
		return *this;
	}
	Z80Word& operator--()
	{
		--_data;
		MaskData();
		return *this;
	}
	Z80Word operator++(int)
	{
		Z80Word temp(*this);
		++_data;
		MaskData();
		return temp;
	}
	Z80Word operator--(int)
	{
		Z80Word temp(*this);
		--_data;
		MaskData();
		return temp;
	}
};

} // Close namespace Z80
#endif
