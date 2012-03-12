//----------------------------------------------------------------------------------------
template<class T> void IntToString(T& data, std::wstring& string)
{
	IntToStringBase10(data, string);
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase16(T data, std::wstring& string, unsigned int charLength, bool includePrefix)
{
	string.clear();
	unsigned int bitsInChar = 4;
	unsigned int bitMask = 0xF;
	unsigned int chars = charLength;
	if(charLength == 0)
	{
		chars = (((unsigned int)sizeof(data) * CHAR_BIT) + (bitsInChar - 1)) / bitsInChar;
	}
	if(data < 0)
	{
		string.push_back(L'-');
		data = 0 - data;
	}
	if(includePrefix)
	{
		string += L"0x";
	}
	for(unsigned int i = 0; i < chars; ++i)
	{
		unsigned int nybble = (unsigned int)(data >> (((chars - 1) - i) * bitsInChar)) & bitMask;
		string.push_back(HexNybbleToWChar(nybble));
	}
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase10(T data, std::wstring& string)
{
	string.clear();
	bool negative = false;
	if(data < 0)
	{
		negative = true;
		data = 0 - data;
	}
	while(data > 0)
	{
		unsigned int digit = data % 10;
		string.insert(string.begin(), HexNybbleToWChar(digit));
		data /= 10;
	}
	if(negative)
	{
		string.insert(string.begin(), L'-');
	}
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase8(T data, std::wstring& string, bool includePrefix)
{
	string.clear();
	unsigned int bitsInChar = 3;
	unsigned int bitMask = 0x7;
	unsigned int chars = (((unsigned int)sizeof(data) * CHAR_BIT) + (bitsInChar - 1)) / bitsInChar;
	if(data < 0)
	{
		string.push_back(L'-');
		data = 0 - data;
	}
	for(unsigned int i = 0; i < chars; ++i)
	{
		unsigned int nybble = (unsigned int)(data >> (((chars - 1) - i) * bitsInChar)) & bitMask;
		string.push_back(HexNybbleToWChar(nybble));
	}
	if(includePrefix)
	{
		string += L"o";
	}
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase2(T data, std::wstring& string, unsigned int charLength, bool includePrefix)
{
	string.clear();
	unsigned int bitsInChar = 1;
	unsigned int bitMask = 0x1;
	unsigned int chars = charLength;
	if(charLength == 0)
	{
		chars = (((unsigned int)sizeof(data) * CHAR_BIT) + (bitsInChar - 1)) / bitsInChar;
	}
	if(data < 0)
	{
		string.push_back(L'-');
		data = 0 - data;
	}
	for(unsigned int i = 0; i < chars; ++i)
	{
		unsigned int nybble = (unsigned int)(data >> (((chars - 1) - i) * bitsInChar)) & bitMask;
		string.push_back(HexNybbleToWChar(nybble));
	}
	if(includePrefix)
	{
		string += L"b";
	}
}

//----------------------------------------------------------------------------------------
template<class T> void StringToInt(const std::wstring& string, T& data)
{
	if(string.empty())
	{
		data = 0;
		return;
	}

	//Check for a prefix
	unsigned int prefixChar = 0;
	if(string[prefixChar] == L'-')
	{
		++prefixChar;
		if(prefixChar >= string.length())
		{
			data = 0;
			return;
		}
	}
	if(string[prefixChar] == L'0')
	{
		++prefixChar;
		if(prefixChar >= string.length())
		{
			data = 0;
			return;
		}
	}
	switch(string[prefixChar])
	{
	case L'X':
	case L'x':
	case L'H':
	case L'h':
		StringToIntBase16(string, data);
		return;
	case L'O':
	case L'o':
	case L'Q':
	case L'q':
		StringToIntBase8(string, data);
		return;
	case L'B':
	case L'b':
		StringToIntBase2(string, data);
		return;
	}

	//Check for a postfix
	unsigned int postfixChar = (unsigned int)string.length() - 1;
	switch(postfixChar)
	{
	case L'X':
	case L'x':
	case L'H':
	case L'h':
		StringToIntBase16(string, data);
		return;
	case L'O':
	case L'o':
	case L'Q':
	case L'q':
		StringToIntBase8(string, data);
		return;
	case L'B':
	case L'b':
		StringToIntBase2(string, data);
		return;
	}

	//Default to base 10
	StringToIntBase10(string, data);
	return;
}

//----------------------------------------------------------------------------------------
template<class T> void StringToIntBase16(const std::wstring& string, T& data)
{
	bool negative = (string[0] == L'-');

	data = 0;
	unsigned int bitsInChar = 4;
	for(unsigned int i = 0; i < string.length(); ++i)
	{
		unsigned int digit = HexWCharToNybble(string[i]);
		if(digit < 0x10)
		{
			data = (data << bitsInChar) | digit;
		}
	}

	if(negative)
	{
		data = T(0) - data;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void StringToIntBase10(const std::wstring& string, T& data)
{
	bool negative = (string[0] == L'-');

	data = 0;
	unsigned int mul = 1;
	for(unsigned int i = 0; i < string.length(); ++i)
	{
		unsigned int digit = HexWCharToNybble(string[(string.length() - 1) - i]);
		if(digit < 0xA)
		{
			data += digit * mul;
			mul *= 10;
		}
	}

	if(negative)
	{
		data = T(0) - data;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void StringToIntBase8(const std::wstring& string, T& data)
{
	bool negative = (string[0] == L'-');

	data = 0;
	unsigned int bitsInChar = 3;
	for(unsigned int i = 0; i < string.length(); ++i)
	{
		unsigned int digit = HexWCharToNybble(string[i]);
		if(digit < 0x8)
		{
			data = (data << bitsInChar) | digit;
		}
	}

	if(negative)
	{
		data = T(0) - data;
	}
}

//----------------------------------------------------------------------------------------
template<class T> void StringToIntBase2(const std::wstring& string, T& data)
{
	bool negative = (string[0] == L'-');

	data = 0;
	unsigned int bitsInChar = 1;
	for(unsigned int i = 0; i < string.length(); ++i)
	{
		unsigned int digit = HexWCharToNybble(string[i]);
		if(digit < 0x2)
		{
			data = (data << bitsInChar) | digit;
		}
	}

	if(negative)
	{
		data = T(0) - data;
	}
}
