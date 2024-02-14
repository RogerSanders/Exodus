#ifndef __M68000_DATA_H__
#define __M68000_DATA_H__
namespace M68000 {

enum Bitcount
{
	BITCOUNT_BYTE = 8,
	BITCOUNT_WORD = 16,
	BITCOUNT_LONG = 32
};

//##TODO## Figure out a way to get the same behaviour we currently have with these custom
// types, without the need to duplicate all this operator code here.

class M68000Byte :public Data
{
public:
	// Constructors
	inline M68000Byte()
	:Data(BITCOUNT_BYTE)
	{ }
	inline M68000Byte(unsigned int data)
	:Data(BITCOUNT_BYTE, data)
	{ }
	inline explicit M68000Byte(const Data& data)
	:Data(BITCOUNT_BYTE, data.GetData())
	{ }
	inline M68000Byte(const M68000Byte& source) = default;

	// Integer operators
	M68000Byte operator+(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp += target;
		return temp;
	}
	M68000Byte operator-(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp -= target;
		return temp;
	}
	M68000Byte operator*(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp *= target;
		return temp;
	}
	M68000Byte operator/(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp /= target;
		return temp;
	}
	M68000Byte operator&(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp &= target;
		return temp;
	}
	M68000Byte operator|(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp |= target;
		return temp;
	}
	M68000Byte operator^(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp ^= target;
		return temp;
	}
	M68000Byte operator%(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp %= target;
		return temp;
	}
	M68000Byte operator<<(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp <<= target;
		return temp;
	}
	M68000Byte operator>>(unsigned int target) const
	{
		M68000Byte temp(*this);
		temp >>= target;
		return temp;
	}
	M68000Byte& operator=(unsigned int target)
	{
		_data = target;
		MaskData();
		return *this;
	}
	M68000Byte& operator+=(unsigned int target)
	{
		_data += target;
		MaskData();
		return *this;
	}
	M68000Byte& operator-=(unsigned int target)
	{
		_data -= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator*=(unsigned int target)
	{
		_data *= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator/=(unsigned int target)
	{
		_data /= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator&=(unsigned int target)
	{
		_data &= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator|=(unsigned int target)
	{
		_data |= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator^=(unsigned int target)
	{
		_data ^= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator%=(unsigned int target)
	{
		_data %= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator<<=(unsigned int target)
	{
		_data <<= target;
		MaskData();
		return *this;
	}
	M68000Byte& operator>>=(unsigned int target)
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

	// M68000Byte operators
	M68000Byte operator+(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp += target;
		return temp;
	}
	M68000Byte operator-(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp -= target;
		return temp;
	}
	M68000Byte operator*(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp *= target;
		return temp;
	}
	M68000Byte operator/(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp /= target;
		return temp;
	}
	M68000Byte operator&(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp &= target;
		return temp;
	}
	M68000Byte operator|(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp |= target;
		return temp;
	}
	M68000Byte operator^(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp ^= target;
		return temp;
	}
	M68000Byte operator%(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp %= target;
		return temp;
	}
	M68000Byte operator<<(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp <<= target;
		return temp;
	}
	M68000Byte operator>>(const M68000Byte& target) const
	{
		M68000Byte temp(*this);
		temp >>= target;
		return temp;
	}
	M68000Byte& operator=(const M68000Byte& target)
	{
		_data = target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator+=(const M68000Byte& target)
	{
		_data += target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator-=(const M68000Byte& target)
	{
		_data -= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator*=(const M68000Byte& target)
	{
		_data *= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator/=(const M68000Byte& target)
	{
		_data /= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator&=(const M68000Byte& target)
	{
		_data &= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator|=(const M68000Byte& target)
	{
		_data |= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator^=(const M68000Byte& target)
	{
		_data ^= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator%=(const M68000Byte& target)
	{
		_data %= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator<<=(const M68000Byte& target)
	{
		_data <<= target._data;
		MaskData();
		return *this;
	}
	M68000Byte& operator>>=(const M68000Byte& target)
	{
		_data >>= target._data;
		MaskData();
		return *this;
	}
	bool operator==(const M68000Byte& target) const
	{
		return _data == target._data;
	}
	bool operator!=(const M68000Byte& target) const
	{
		return _data != target._data;
	}
	bool operator>(const M68000Byte& target) const
	{
		return _data > target._data;
	}
	bool operator<(const M68000Byte& target) const
	{
		return _data < target._data;
	}
	bool operator>=(const M68000Byte& target) const
	{
		return _data >= target._data;
	}
	bool operator<=(const M68000Byte& target) const
	{
		return _data <= target._data;
	}

	// Unary operators
	M68000Byte operator~() const
	{
		M68000Byte temp(*this);
		temp._data = ~temp._data;
		temp.MaskData();
		return temp;
	}
	M68000Byte& operator++()
	{
		++_data;
		MaskData();
		return *this;
	}
	M68000Byte& operator--()
	{
		--_data;
		MaskData();
		return *this;
	}
	M68000Byte operator++(int)
	{
		M68000Byte temp(*this);
		++_data;
		MaskData();
		return temp;
	}
	M68000Byte operator--(int)
	{
		M68000Byte temp(*this);
		--_data;
		MaskData();
		return temp;
	}
};

class M68000Word :public Data
{
public:
	// Constructors
	inline M68000Word()
	:Data(BITCOUNT_WORD)
	{ }
	inline M68000Word(unsigned int data)
	:Data(BITCOUNT_WORD, data)
	{ }
	inline explicit M68000Word(const Data& data)
	:Data(BITCOUNT_WORD, data.GetData())
	{ }
	inline M68000Word(const M68000Word& source) = default;

	// Integer operators
	M68000Word operator+(unsigned int target) const
	{
		M68000Word temp(*this);
		temp += target;
		return temp;
	}
	M68000Word operator-(unsigned int target) const
	{
		M68000Word temp(*this);
		temp -= target;
		return temp;
	}
	M68000Word operator*(unsigned int target) const
	{
		M68000Word temp(*this);
		temp *= target;
		return temp;
	}
	M68000Word operator/(unsigned int target) const
	{
		M68000Word temp(*this);
		temp /= target;
		return temp;
	}
	M68000Word operator&(unsigned int target) const
	{
		M68000Word temp(*this);
		temp &= target;
		return temp;
	}
	M68000Word operator|(unsigned int target) const
	{
		M68000Word temp(*this);
		temp |= target;
		return temp;
	}
	M68000Word operator^(unsigned int target) const
	{
		M68000Word temp(*this);
		temp ^= target;
		return temp;
	}
	M68000Word operator%(unsigned int target) const
	{
		M68000Word temp(*this);
		temp %= target;
		return temp;
	}
	M68000Word operator<<(unsigned int target) const
	{
		M68000Word temp(*this);
		temp <<= target;
		return temp;
	}
	M68000Word operator>>(unsigned int target) const
	{
		M68000Word temp(*this);
		temp >>= target;
		return temp;
	}
	M68000Word& operator=(unsigned int target)
	{
		_data = target;
		MaskData();
		return *this;
	}
	M68000Word& operator+=(unsigned int target)
	{
		_data += target;
		MaskData();
		return *this;
	}
	M68000Word& operator-=(unsigned int target)
	{
		_data -= target;
		MaskData();
		return *this;
	}
	M68000Word& operator*=(unsigned int target)
	{
		_data *= target;
		MaskData();
		return *this;
	}
	M68000Word& operator/=(unsigned int target)
	{
		_data /= target;
		MaskData();
		return *this;
	}
	M68000Word& operator&=(unsigned int target)
	{
		_data &= target;
		MaskData();
		return *this;
	}
	M68000Word& operator|=(unsigned int target)
	{
		_data |= target;
		MaskData();
		return *this;
	}
	M68000Word& operator^=(unsigned int target)
	{
		_data ^= target;
		MaskData();
		return *this;
	}
	M68000Word& operator%=(unsigned int target)
	{
		_data %= target;
		MaskData();
		return *this;
	}
	M68000Word& operator<<=(unsigned int target)
	{
		_data <<= target;
		MaskData();
		return *this;
	}
	M68000Word& operator>>=(unsigned int target)
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

	// M68000Word operators
	M68000Word operator+(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp += target;
		return temp;
	}
	M68000Word operator-(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp -= target;
		return temp;
	}
	M68000Word operator*(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp *= target;
		return temp;
	}
	M68000Word operator/(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp /= target;
		return temp;
	}
	M68000Word operator&(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp &= target;
		return temp;
	}
	M68000Word operator|(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp |= target;
		return temp;
	}
	M68000Word operator^(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp ^= target;
		return temp;
	}
	M68000Word operator%(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp %= target;
		return temp;
	}
	M68000Word operator<<(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp <<= target;
		return temp;
	}
	M68000Word operator>>(const M68000Word& target) const
	{
		M68000Word temp(*this);
		temp >>= target;
		return temp;
	}
	M68000Word& operator=(const M68000Word& target)
	{
		_data = target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator+=(const M68000Word& target)
	{
		_data += target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator-=(const M68000Word& target)
	{
		_data -= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator*=(const M68000Word& target)
	{
		_data *= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator/=(const M68000Word& target)
	{
		_data /= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator&=(const M68000Word& target)
	{
		_data &= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator|=(const M68000Word& target)
	{
		_data |= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator^=(const M68000Word& target)
	{
		_data ^= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator%=(const M68000Word& target)
	{
		_data %= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator<<=(const M68000Word& target)
	{
		_data <<= target._data;
		MaskData();
		return *this;
	}
	M68000Word& operator>>=(const M68000Word& target)
	{
		_data >>= target._data;
		MaskData();
		return *this;
	}
	bool operator==(const M68000Word& target) const
	{
		return _data == target._data;
	}
	bool operator!=(const M68000Word& target) const
	{
		return _data != target._data;
	}
	bool operator>(const M68000Word& target) const
	{
		return _data > target._data;
	}
	bool operator<(const M68000Word& target) const
	{
		return _data < target._data;
	}
	bool operator>=(const M68000Word& target) const
	{
		return _data >= target._data;
	}
	bool operator<=(const M68000Word& target) const
	{
		return _data <= target._data;
	}

	// Unary operators
	M68000Word operator~() const
	{
		M68000Word temp(*this);
		temp._data = ~temp._data;
		temp.MaskData();
		return temp;
	}
	M68000Word& operator++()
	{
		++_data;
		MaskData();
		return *this;
	}
	M68000Word& operator--()
	{
		--_data;
		MaskData();
		return *this;
	}
	M68000Word operator++(int)
	{
		M68000Word temp(*this);
		++_data;
		MaskData();
		return temp;
	}
	M68000Word operator--(int)
	{
		M68000Word temp(*this);
		--_data;
		MaskData();
		return temp;
	}
};

class M68000Long :public Data
{
public:
	// Constructors
	inline M68000Long()
	:Data(BITCOUNT_LONG)
	{ }
	inline M68000Long(unsigned int data)
	:Data(BITCOUNT_LONG, data)
	{ }
	inline explicit M68000Long(const Data& data)
	:Data(BITCOUNT_LONG, data.GetData())
	{ }
	inline M68000Long(const M68000Long& source) = default;

	// Integer operators
	M68000Long operator+(unsigned int target) const
	{
		M68000Long temp(*this);
		temp += target;
		return temp;
	}
	M68000Long operator-(unsigned int target) const
	{
		M68000Long temp(*this);
		temp -= target;
		return temp;
	}
	M68000Long operator*(unsigned int target) const
	{
		M68000Long temp(*this);
		temp *= target;
		return temp;
	}
	M68000Long operator/(unsigned int target) const
	{
		M68000Long temp(*this);
		temp /= target;
		return temp;
	}
	M68000Long operator&(unsigned int target) const
	{
		M68000Long temp(*this);
		temp &= target;
		return temp;
	}
	M68000Long operator|(unsigned int target) const
	{
		M68000Long temp(*this);
		temp |= target;
		return temp;
	}
	M68000Long operator^(unsigned int target) const
	{
		M68000Long temp(*this);
		temp ^= target;
		return temp;
	}
	M68000Long operator%(unsigned int target) const
	{
		M68000Long temp(*this);
		temp %= target;
		return temp;
	}
	M68000Long operator<<(unsigned int target) const
	{
		M68000Long temp(*this);
		temp <<= target;
		return temp;
	}
	M68000Long operator>>(unsigned int target) const
	{
		M68000Long temp(*this);
		temp >>= target;
		return temp;
	}
	M68000Long& operator=(unsigned int target)
	{
		_data = target;
		MaskData();
		return *this;
	}
	M68000Long& operator+=(unsigned int target)
	{
		_data += target;
		MaskData();
		return *this;
	}
	M68000Long& operator-=(unsigned int target)
	{
		_data -= target;
		MaskData();
		return *this;
	}
	M68000Long& operator*=(unsigned int target)
	{
		_data *= target;
		MaskData();
		return *this;
	}
	M68000Long& operator/=(unsigned int target)
	{
		_data /= target;
		MaskData();
		return *this;
	}
	M68000Long& operator&=(unsigned int target)
	{
		_data &= target;
		MaskData();
		return *this;
	}
	M68000Long& operator|=(unsigned int target)
	{
		_data |= target;
		MaskData();
		return *this;
	}
	M68000Long& operator^=(unsigned int target)
	{
		_data ^= target;
		MaskData();
		return *this;
	}
	M68000Long& operator%=(unsigned int target)
	{
		_data %= target;
		MaskData();
		return *this;
	}
	M68000Long& operator<<=(unsigned int target)
	{
		_data <<= target;
		MaskData();
		return *this;
	}
	M68000Long& operator>>=(unsigned int target)
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

	// M68000Long operators
	M68000Long operator+(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp += target;
		return temp;
	}
	M68000Long operator-(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp -= target;
		return temp;
	}
	M68000Long operator*(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp *= target;
		return temp;
	}
	M68000Long operator/(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp /= target;
		return temp;
	}
	M68000Long operator&(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp &= target;
		return temp;
	}
	M68000Long operator|(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp |= target;
		return temp;
	}
	M68000Long operator^(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp ^= target;
		return temp;
	}
	M68000Long operator%(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp %= target;
		return temp;
	}
	M68000Long operator<<(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp <<= target;
		return temp;
	}
	M68000Long operator>>(const M68000Long& target) const
	{
		M68000Long temp(*this);
		temp >>= target;
		return temp;
	}
	M68000Long& operator=(const M68000Long& target)
	{
		_data = target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator+=(const M68000Long& target)
	{
		_data += target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator-=(const M68000Long& target)
	{
		_data -= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator*=(const M68000Long& target)
	{
		_data *= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator/=(const M68000Long& target)
	{
		_data /= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator&=(const M68000Long& target)
	{
		_data &= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator|=(const M68000Long& target)
	{
		_data |= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator^=(const M68000Long& target)
	{
		_data ^= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator%=(const M68000Long& target)
	{
		_data %= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator<<=(const M68000Long& target)
	{
		_data <<= target._data;
		MaskData();
		return *this;
	}
	M68000Long& operator>>=(const M68000Long& target)
	{
		_data >>= target._data;
		MaskData();
		return *this;
	}
	bool operator==(const M68000Long& target) const
	{
		return _data == target._data;
	}
	bool operator!=(const M68000Long& target) const
	{
		return _data != target._data;
	}
	bool operator>(const M68000Long& target) const
	{
		return _data > target._data;
	}
	bool operator<(const M68000Long& target) const
	{
		return _data < target._data;
	}
	bool operator>=(const M68000Long& target) const
	{
		return _data >= target._data;
	}
	bool operator<=(const M68000Long& target) const
	{
		return _data <= target._data;
	}

	// Unary operators
	M68000Long operator~() const
	{
		M68000Long temp(*this);
		temp._data = ~temp._data;
		temp.MaskData();
		return temp;
	}
	M68000Long& operator++()
	{
		++_data;
		MaskData();
		return *this;
	}
	M68000Long& operator--()
	{
		--_data;
		MaskData();
		return *this;
	}
	M68000Long operator++(int)
	{
		M68000Long temp(*this);
		++_data;
		MaskData();
		return temp;
	}
	M68000Long operator--(int)
	{
		M68000Long temp(*this);
		--_data;
		MaskData();
		return temp;
	}
};

} // Close namespace M68000
#endif
