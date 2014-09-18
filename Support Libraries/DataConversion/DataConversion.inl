#include <sstream>
#include <iomanip>

//----------------------------------------------------------------------------------------
char HexNybbleToChar(unsigned int nybble)
{
	const char conversionTable[0x10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	return conversionTable[nybble & 0xF];
}

//----------------------------------------------------------------------------------------
wchar_t HexNybbleToWChar(unsigned int nybble)
{
	const wchar_t conversionTable[0x10] = {L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F'};
	return conversionTable[nybble & 0xF];
}

//----------------------------------------------------------------------------------------
unsigned int HexCharToNybble(char nybble)
{
	switch(nybble)
	{
	case '0':
		return 0x0;
	case '1':
		return 0x1;
	case '2':
		return 0x2;
	case '3':
		return 0x3;
	case '4':
		return 0x4;
	case '5':
		return 0x5;
	case '6':
		return 0x6;
	case '7':
		return 0x7;
	case '8':
		return 0x8;
	case '9':
		return 0x9;
	case 'A':
	case 'a':
		return 0xA;
	case 'B':
	case 'b':
		return 0xB;
	case 'C':
	case 'c':
		return 0xC;
	case 'D':
	case 'd':
		return 0xD;
	case 'E':
	case 'e':
		return 0xE;
	case 'F':
	case 'f':
		return 0xF;
	}
	return 0x10;
}

//----------------------------------------------------------------------------------------
unsigned int HexWCharToNybble(wchar_t nybble)
{
	switch(nybble)
	{
	case L'0':
		return 0x0;
	case L'1':
		return 0x1;
	case L'2':
		return 0x2;
	case L'3':
		return 0x3;
	case L'4':
		return 0x4;
	case L'5':
		return 0x5;
	case L'6':
		return 0x6;
	case L'7':
		return 0x7;
	case L'8':
		return 0x8;
	case L'9':
		return 0x9;
	case L'A':
	case L'a':
		return 0xA;
	case L'B':
	case L'b':
		return 0xB;
	case L'C':
	case L'c':
		return 0xC;
	case L'D':
	case L'd':
		return 0xD;
	case L'E':
	case L'e':
		return 0xE;
	case L'F':
	case L'f':
		return 0xF;
	}
	return 0x10;
}

//----------------------------------------------------------------------------------------
std::vector<std::string> TokenizeString(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;

	size_t p0 = 0;
	size_t p1 = std::string::npos;
	while(p0 != std::string::npos)
	{
		p1 = str.find_first_of(delim, p0);
		if(p1 != p0)
		{
			std::string token = str.substr(p0, p1 - p0);
			tokens.push_back(token);
		}
		p0 = str.find_first_not_of(delim, p1);
	}

	return tokens;
}

//----------------------------------------------------------------------------------------
std::vector<std::wstring> TokenizeString(const std::wstring& str, const std::wstring& delim)
{
	std::vector<std::wstring> tokens;

	size_t p0 = 0;
	size_t p1 = std::wstring::npos;
	while(p0 != std::wstring::npos)
	{
		p1 = str.find_first_of(delim, p0);
		if(p1 != p0)
		{
			std::wstring token = str.substr(p0, p1 - p0);
			tokens.push_back(token);
		}
		p0 = str.find_first_not_of(delim, p1);
	}

	return tokens;
}

//----------------------------------------------------------------------------------------
template<class T> bool IntToString(T data, std::wstring& string, unsigned int stringBase)
{
	//Attempt to convert the number to a string using the specified base
	switch(stringBase)
	{
	case 16:
		IntToStringBase16(data, string, 0, true);
		return true;
	case 10:
		IntToStringBase10(data, string, 0, false);
		return true;
	case 8:
		IntToStringBase8(data, string, 0, true);
		return true;
	case 2:
		IntToStringBase2(data, string, 0, true);
		return true;
	}

	//If the specified base was not supported, return false.
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase16(T data, std::wstring& string, unsigned int charLength, bool encodeType)
{
	//Set some constants for the requested output base type
	unsigned int bitsInChar = 4;
	unsigned int bitMask = 0xF;
	wchar_t prefixChar = L'x';

	//If the number is negative, flag that we're working with a negative number, and
	//convert it to a positive number. Note that there's a potential overflow here with
	//the negative most number, IE, -0x80 in the case of an 8-bit integer. This doesn't
	//cause an error, since -0x80 also has a binary representation of 0x80 in memory, so
	//we can use this method and still get the correct result. Also note that this assumes
	//two's compliment representation of integers, which is platform dependent and not
	//guaranteed by the C++ standard, but is a pretty safe assumption now and into the
	//future.
	bool negative = false;
	if(data < 0)
	{
		negative = true;
		data = 0 - data;
	}

	//Start to build the string representation of the input data, writing the data in
	//reverse. We build the string in reverse because it's hard to know the final required
	//length of the string ahead of time, and this avoids the need to push data to the
	//front of the string as we build it, which would be less efficient.
	std::wstring reverseString;

	//Convert the input data to a string one character at a time, until the data reaches a
	//value of 0. Note that we ensure here that a value of 0 outputs a character as well.
	unsigned int chars = 0;
	do
	{
		unsigned int nybble = (unsigned int)(data & bitMask);
		reverseString.push_back(HexNybbleToWChar(nybble));
		data >>= bitsInChar;
		++chars;
	}
	while(data != 0);

	//Pad out the resulting string to the requested minimum character length
	for(unsigned int i = chars; i < charLength; ++i)
	{
		reverseString.push_back(L'0');
	}

	//Apply a prefix to the string if requested
	if(encodeType)
	{
		reverseString.push_back(prefixChar);
		reverseString.push_back(L'0');
	}

	//Add the negative sign to the string if required
	if(negative)
	{
		reverseString.push_back(L'-');
	}

	//Write the reverse string to the output string in the correct order
	string.assign(reverseString.rbegin(), reverseString.rend());
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase10(T data, std::wstring& string, unsigned int charLength, bool encodeType)
{
	//Set some constants for the requested output base type
	wchar_t prefixChar = L'd';

	//If the number is negative, flag that we're working with a negative number, and
	//convert it to a positive number. Note that there's a potential overflow here with
	//the negative most number, IE, -0x80 in the case of an 8-bit integer. This doesn't
	//cause an error, since -0x80 also has a binary representation of 0x80 in memory, so
	//we can use this method and still get the correct result. Also note that this assumes
	//two's compliment representation of integers, which is platform dependent and not
	//guaranteed by the C++ standard, but is a pretty safe assumption now and into the
	//future.
	bool negative = false;
	if(data < 0)
	{
		negative = true;
		data = 0 - data;
	}

	//Start to build the string representation of the input data, writing the data in
	//reverse. We build the string in reverse because it's hard to know the final required
	//length of the string ahead of time, and this avoids the need to push data to the
	//front of the string as we build it, which would be less efficient.
	std::wstring reverseString;

	//Convert the input data to a string one character at a time, until the data reaches a
	//value of 0. Note that we ensure here that a value of 0 outputs a character as well.
	unsigned int chars = 0;
	do
	{
		unsigned int digit = (unsigned int)(data % 10);
		reverseString.push_back(HexNybbleToWChar(digit));
		data /= 10;
		++chars;
	}
	while(data != 0);

	//Pad out the resulting string to the requested minimum character length
	for(unsigned int i = chars; i < charLength; ++i)
	{
		reverseString.push_back(L'0');
	}

	//Apply a prefix to the string if requested
	if(encodeType)
	{
		reverseString.push_back(prefixChar);
		reverseString.push_back(L'0');
	}

	//Add the negative sign to the string if required
	if(negative)
	{
		reverseString.push_back(L'-');
	}

	//Write the reverse string to the output string in the correct order
	string.assign(reverseString.rbegin(), reverseString.rend());
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase8(T data, std::wstring& string, unsigned int charLength, bool encodeType)
{
	//Set some constants for the requested output base type
	unsigned int bitsInChar = 3;
	unsigned int bitMask = 0x7;
	wchar_t prefixChar = L'o';

	//If the number is negative, flag that we're working with a negative number, and
	//convert it to a positive number. Note that there's a potential overflow here with
	//the negative most number, IE, -0x80 in the case of an 8-bit integer. This doesn't
	//cause an error, since -0x80 also has a binary representation of 0x80 in memory, so
	//we can use this method and still get the correct result. Also note that this assumes
	//two's compliment representation of integers, which is platform dependent and not
	//guaranteed by the C++ standard, but is a pretty safe assumption now and into the
	//future.
	bool negative = false;
	if(data < 0)
	{
		negative = true;
		data = 0 - data;
	}

	//Start to build the string representation of the input data, writing the data in
	//reverse. We build the string in reverse because it's hard to know the final required
	//length of the string ahead of time, and this avoids the need to push data to the
	//front of the string as we build it, which would be less efficient.
	std::wstring reverseString;

	//Convert the input data to a string one character at a time, until the data reaches a
	//value of 0. Note that we ensure here that a value of 0 outputs a character as well.
	unsigned int chars = 0;
	do
	{
		unsigned int nybble = (unsigned int)(data & bitMask);
		reverseString.push_back(HexNybbleToWChar(nybble));
		data >>= bitsInChar;
		++chars;
	}
	while(data != 0);

	//Pad out the resulting string to the requested minimum character length
	for(unsigned int i = chars; i < charLength; ++i)
	{
		reverseString.push_back(L'0');
	}

	//Apply a prefix to the string if requested
	if(encodeType)
	{
		reverseString.push_back(prefixChar);
		reverseString.push_back(L'0');
	}

	//Add the negative sign to the string if required
	if(negative)
	{
		reverseString.push_back(L'-');
	}

	//Write the reverse string to the output string in the correct order
	string.assign(reverseString.rbegin(), reverseString.rend());
}

//----------------------------------------------------------------------------------------
template<class T> void IntToStringBase2(T data, std::wstring& string, unsigned int charLength, bool encodeType)
{
	//Set some constants for the requested output base type
	unsigned int bitsInChar = 1;
	unsigned int bitMask = 0x1;
	wchar_t prefixChar = L'b';

	//If the number is negative, flag that we're working with a negative number, and
	//convert it to a positive number. Note that there's a potential overflow here with
	//the negative most number, IE, -0x80 in the case of an 8-bit integer. This doesn't
	//cause an error, since -0x80 also has a binary representation of 0x80 in memory, so
	//we can use this method and still get the correct result. Also note that this assumes
	//two's compliment representation of integers, which is platform dependent and not
	//guaranteed by the C++ standard, but is a pretty safe assumption now and into the
	//future.
	bool negative = false;
	if(data < 0)
	{
		negative = true;
		data = 0 - data;
	}

	//Start to build the string representation of the input data, writing the data in
	//reverse. We build the string in reverse because it's hard to know the final required
	//length of the string ahead of time, and this avoids the need to push data to the
	//front of the string as we build it, which would be less efficient.
	std::wstring reverseString;

	//Convert the input data to a string one character at a time, until the data reaches a
	//value of 0. Note that we ensure here that a value of 0 outputs a character as well.
	unsigned int chars = 0;
	do
	{
		unsigned int nybble = (unsigned int)(data & bitMask);
		reverseString.push_back(HexNybbleToWChar(nybble));
		data >>= bitsInChar;
		++chars;
	}
	while(data != 0);

	//Pad out the resulting string to the requested minimum character length
	for(unsigned int i = chars; i < charLength; ++i)
	{
		reverseString.push_back(L'0');
	}

	//Apply a prefix to the string if requested
	if(encodeType)
	{
		reverseString.push_back(prefixChar);
		reverseString.push_back(L'0');
	}

	//Add the negative sign to the string if required
	if(negative)
	{
		reverseString.push_back(L'-');
	}

	//Write the reverse string to the output string in the correct order
	string.assign(reverseString.rbegin(), reverseString.rend());
}

//----------------------------------------------------------------------------------------
template<class T> bool StringToInt(const std::wstring& string, T& data, unsigned int defaultBase)
{
	//Abort if the string is empty
	size_t stringLength = string.length();
	if(stringLength <= 0)
	{
		return false;
	}

	//Check for a prefix, and parse the string as the indicated type if present.
	size_t startPos = (string[0] == L'-')? 1: 0;
	if(((startPos + 2) < stringLength) && (string[startPos] == L'0'))
	{
		wchar_t typePrefix = CharToLower(string[startPos + 1]);
		switch(typePrefix)
		{
		case L'x':
		case L'h':
			return StringToIntBase16(string, data);
		case L'd':
			return StringToIntBase10(string, data);
		case L'o':
		case L'q':
			return StringToIntBase8(string, data);
		case L'b':
			return StringToIntBase2(string, data);
		}
	}

	//Attempt to parse the string with the specified default base
	switch(defaultBase)
	{
	case 16:
		return StringToIntBase16(string, data);
	case 10:
		return StringToIntBase10(string, data);
	case 8:
		return StringToIntBase8(string, data);
	case 2:
		return StringToIntBase2(string, data);
	}

	//If the base of the specified number could not be determined, return false.
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool StringToIntBase16(const std::wstring& string, T& data)
{
	//Abort if the string is empty
	size_t stringLength = string.length();
	if(stringLength <= 0)
	{
		return false;
	}

	//Extract any prefix on the data
	bool negative = (string[0] == L'-');
	size_t startPos = (negative)? 1: 0;
	if(((startPos + 2) < stringLength) && (string[startPos] == L'0'))
	{
		wchar_t typePrefix = CharToLower(string[startPos + 1]);
		if((typePrefix == L'h') || (typePrefix == L'x'))
		{
			startPos += 2;
		}
	}

	//Decode the data
	T decodedData = 0;
	static const unsigned int bitsInChar = 4;
	for(size_t i = startPos; i < stringLength; ++i)
	{
		//Decode this digit from the string
		wchar_t nextChar = string[i];
		T digit = (T)HexWCharToNybble(nextChar);
		if(digit > 0xF)
		{
			return false;
		}

		//Combine this digit into the decoded data
		decodedData = (decodedData << bitsInChar) | digit;
	}

	//If this number was prefixed with a minus sign, negate the result.
	if(negative)
	{
		decodedData = T(0) - decodedData;
	}

	//Return the decoded data to the caller
	data = decodedData;
	return true;
}

//----------------------------------------------------------------------------------------
template<class T> bool StringToIntBase10(const std::wstring& string, T& data)
{
	//Abort if the string is empty
	size_t stringLength = string.length();
	if(stringLength <= 0)
	{
		return false;
	}

	//Extract any prefix on the data
	bool negative = (string[0] == L'-');
	size_t startPos = (negative)? 1: 0;
	if(((startPos + 2) < stringLength) && (string[startPos] == L'0'))
	{
		wchar_t typePrefix = CharToLower(string[startPos + 1]);
		if(typePrefix == L'd')
		{
			startPos += 2;
		}
	}

	//Decode the data
	T decodedData = 0;
	for(size_t i = startPos; i < stringLength; ++i)
	{
		//Decode this digit from the string
		wchar_t nextChar = string[i];
		T digit = (T)HexWCharToNybble(nextChar);
		if(digit > 9)
		{
			return false;
		}

		//Combine this digit into the decoded data
		decodedData = (decodedData * 10) + digit;
	}

	//If this number was prefixed with a minus sign, negate the result.
	if(negative)
	{
		decodedData = T(0) - decodedData;
	}

	//Return the decoded data to the caller
	data = decodedData;
	return true;
}

//----------------------------------------------------------------------------------------
template<class T> bool StringToIntBase8(const std::wstring& string, T& data)
{
	//Abort if the string is empty
	size_t stringLength = string.length();
	if(stringLength <= 0)
	{
		return false;
	}

	//Extract any prefix on the data
	bool negative = (string[0] == L'-');
	size_t startPos = (negative)? 1: 0;
	if(((startPos + 2) < stringLength) && (string[startPos] == L'0'))
	{
		wchar_t typePrefix = CharToLower(string[startPos + 1]);
		if((typePrefix == L'o') || (typePrefix == L'q'))
		{
			startPos += 2;
		}
	}

	//Decode the data
	T decodedData = 0;
	static const unsigned int bitsInChar = 3;
	for(size_t i = startPos; i < stringLength; ++i)
	{
		//Decode this digit from the string
		wchar_t nextChar = string[i];
		T digit = (T)HexWCharToNybble(nextChar);
		if(digit > 7)
		{
			return false;
		}

		//Combine this digit into the decoded data
		decodedData = (decodedData << bitsInChar) | digit;
	}

	//If this number was prefixed with a minus sign, negate the result.
	if(negative)
	{
		decodedData = T(0) - decodedData;
	}

	//Return the decoded data to the caller
	data = decodedData;
	return true;
}

//----------------------------------------------------------------------------------------
template<class T> bool StringToIntBase2(const std::wstring& string, T& data)
{
	//Abort if the string is empty
	size_t stringLength = string.length();
	if(stringLength <= 0)
	{
		return false;
	}

	//Extract any prefix on the data
	bool negative = (string[0] == L'-');
	size_t startPos = (negative)? 1: 0;
	if(((startPos + 2) < stringLength) && (string[startPos] == L'0'))
	{
		wchar_t typePrefix = CharToLower(string[startPos + 1]);
		if(typePrefix == L'b')
		{
			startPos += 2;
		}
	}

	//Decode the data
	T decodedData = 0;
	static const unsigned int bitsInChar = 1;
	for(size_t i = startPos; i < stringLength; ++i)
	{
		//Decode this digit from the string
		wchar_t nextChar = string[i];
		T digit = (T)HexWCharToNybble(nextChar);
		if(digit > 1)
		{
			return false;
		}

		//Combine this digit into the decoded data
		decodedData = (decodedData << bitsInChar) | digit;
	}

	//If this number was prefixed with a minus sign, negate the result.
	if(negative)
	{
		decodedData = T(0) - decodedData;
	}

	//Return the decoded data to the caller
	data = decodedData;
	return true;
}

//----------------------------------------------------------------------------------------
template<class T> void PointerToString(T* pointer, std::wstring& string)
{
	std::wstringstream stringStream;
	stringStream << (void*)pointer;
	string = stringStream.str();
}

//----------------------------------------------------------------------------------------
void BoolToString(bool data, std::wstring& string, bool numericForm)
{
	if(numericForm)
	{
		string = (data)? L"1": L"0";
	}
	else
	{
		string = (data)? L"true": L"false";
	}
}

//----------------------------------------------------------------------------------------
bool StringToBool(const std::wstring& string, bool& data)
{
	std::wstring stringAsLower = StringToLower(string);
	if(string == L"1")
	{
		data = true;
	}
	else if(string == L"0")
	{
		data = false;
	}
	else if(stringAsLower == L"true")
	{
		data = true;
	}
	else if(stringAsLower == L"false")
	{
		data = false;
	}
	else
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------------------
void FloatToString(float data, std::wstring& string, bool scientificNotation, unsigned int fractionalNumberCharLength, unsigned int wholeNumberCharLength)
{
	//std::wstring outputBuffer;
	//outputBuffer.resize(fractionalNumberCharLength + wholeNumberCharLength + 40);

	//std::wstring formatString;
	//formatString += L"%0#";
	//if(wholeNumberCharLength > 0)
	//{
	//	std::wstring wholeNumberCharLengthAsString;
	//	IntToStringBase10(wholeNumberCharLength, wholeNumberCharLengthAsString);
	//	formatString += wholeNumberCharLengthAsString;
	//}
	//if(fractionalNumberCharLength > 0)
	//{
	//	std::wstring fractionalNumberCharLengthAsString;
	//	IntToStringBase10(fractionalNumberCharLength, fractionalNumberCharLengthAsString);
	//	formatString += L'.';
	//	formatString += fractionalNumberCharLengthAsString;
	//}
	//formatString += (scientificNotation)? L'e': L'f';
	//int swprintfReturn = swprintf(&outputBuffer[0], outputBuffer.size(), formatString.c_str(), (double)data);
	//outputBuffer.resize(swprintfReturn);
	//string = outputBuffer;

	std::wstringstream stringStream;
	stringStream << std::showpoint;
	if(scientificNotation)
	{
		stringStream << std::scientific;
	}
	else
	{
		stringStream << std::fixed;
		if(fractionalNumberCharLength > 0)
		{
			stringStream << std::setprecision(fractionalNumberCharLength);
			if(wholeNumberCharLength > 0)
			{
				unsigned int totalDisplayWidth = wholeNumberCharLength + fractionalNumberCharLength + 1 + ((data < 0.0f)? 1: 0);
				stringStream << std::setfill(L'0') << std::setw(totalDisplayWidth);
			}
		}
	}
	stringStream << data;
	string = stringStream.str();
}

//----------------------------------------------------------------------------------------
bool StringToFloat(const std::wstring& string, float& data)
{
	wchar_t* endPtr = 0;
	double result = wcstod(string.c_str(), &endPtr);
	if(*endPtr != L'\0')
	{
		return false;
	}
	data = (float)result;
	return true;
}

//----------------------------------------------------------------------------------------
void DoubleToString(double data, std::wstring& string, bool scientificNotation, unsigned int fractionalNumberCharLength, unsigned int wholeNumberCharLength)
{
	//##FIX## This requires us to convert our manual lengths to strings, and can only
	//operate in double precision.
	//std::wstring outputBuffer;
	//outputBuffer.resize(fractionalNumberCharLength + wholeNumberCharLength + 40);
	//std::wstring formatString;
	//formatString += L"%0#";
	//if(wholeNumberCharLength > 0)
	//{
	//	std::wstring wholeNumberCharLengthAsString;
	//	IntToStringBase10(wholeNumberCharLength, wholeNumberCharLengthAsString);
	//	formatString += wholeNumberCharLengthAsString;
	//}
	//if(fractionalNumberCharLength > 0)
	//{
	//	std::wstring fractionalNumberCharLengthAsString;
	//	IntToStringBase10(fractionalNumberCharLength, fractionalNumberCharLengthAsString);
	//	formatString += L'.';
	//	formatString += fractionalNumberCharLengthAsString;
	//}
	//formatString += (scientificNotation)? L'e': L'f';
	//int swprintfReturn = swprintf(&outputBuffer[0], outputBuffer.size(), formatString.c_str(), data);
	//outputBuffer.resize(swprintfReturn);
	//string = outputBuffer;

	//##FIX## Use printf here instead of a stringstream. Stringstream is slower, and
	//doesn't give us enough control over formatting.
	//##FIX## Both the printf and stringstream methods append trailing 0's, with no option
	//to prevent it.
	//##TODO## Write a completely custom float/double to string method
	std::wstringstream stringStream;
	stringStream << std::showpoint;
	if(scientificNotation)
	{
		stringStream << std::scientific;
	}
	else
	{
		stringStream << std::fixed;
		if(fractionalNumberCharLength > 0)
		{
			stringStream << std::setprecision(fractionalNumberCharLength);
			if(wholeNumberCharLength > 0)
			{
				unsigned int totalDisplayWidth = wholeNumberCharLength + fractionalNumberCharLength + 1 + ((data < 0.0)? 1: 0);
				stringStream << std::setfill(L'0') << std::setw(totalDisplayWidth);
			}
		}
	}
	stringStream << data;
	string = stringStream.str();
}

//----------------------------------------------------------------------------------------
bool StringToDouble(const std::wstring& string, double& data)
{
	wchar_t* endPtr = 0;
	double result = wcstod(string.c_str(), &endPtr);
	if(*endPtr != L'\0')
	{
		return false;
	}
	data = result;
	return true;
}

//----------------------------------------------------------------------------------------
char CharToLower(char data)
{
	static const char toLowerDifference = 'A' - 'a';
	return ((data >= 'A') && (data <= 'Z'))? data + toLowerDifference: data;
}

//----------------------------------------------------------------------------------------
wchar_t CharToLower(wchar_t data)
{
	static const wchar_t toLowerDifference = (wchar_t)(L'A' - L'a');
	return ((data >= L'A') && (data <= L'Z'))? data + toLowerDifference: data;
}

//----------------------------------------------------------------------------------------
char CharToUpper(char data)
{
	static const char toUpperDifference = (wchar_t)('a' - 'A');
	return ((data >= 'a') && (data <= 'z'))? data + toUpperDifference: data;
}

//----------------------------------------------------------------------------------------
wchar_t CharToUpper(wchar_t data)
{
	static const wchar_t toUpperDifference = L'a' - L'A';
	return ((data >= L'a') && (data <= L'z'))? data + toUpperDifference: data;
}

//----------------------------------------------------------------------------------------
std::string StringToLower(const std::string& string)
{
	std::string result;
	size_t stringLength = string.length();
	result.resize(stringLength);
	for(size_t i = 0; i < stringLength; ++i)
	{
		result[i] = CharToLower(string[i]);
	}
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring StringToLower(const std::wstring& string)
{
	std::wstring result;
	size_t stringLength = string.length();
	result.resize(stringLength);
	for(size_t i = 0; i < stringLength; ++i)
	{
		result[i] = CharToLower(string[i]);
	}
	return result;
}

//----------------------------------------------------------------------------------------
std::string StringToUpper(const std::string& string)
{
	std::string result;
	size_t stringLength = string.length();
	result.resize(stringLength);
	for(size_t i = 0; i < stringLength; ++i)
	{
		result[i] = CharToUpper(string[i]);
	}
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring StringToUpper(const std::wstring& string)
{
	std::wstring result;
	size_t stringLength = string.length();
	result.resize(stringLength);
	for(size_t i = 0; i < stringLength; ++i)
	{
		result[i] = CharToUpper(string[i]);
	}
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring StringToWString(const std::string& string)
{
	return std::wstring(string.begin(), string.end());
}

//----------------------------------------------------------------------------------------
std::string WStringToString(const std::wstring& string)
{
	return std::string(string.begin(), string.end());
}
