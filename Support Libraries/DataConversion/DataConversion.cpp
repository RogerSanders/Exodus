#include "DataConversion.h"
#include <climits>

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
