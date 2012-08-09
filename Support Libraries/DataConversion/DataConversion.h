#ifndef __BINARYSTRING_H__
#define __BINARYSTRING_H__
#include <string>
#include <vector>

//##TODO## Implement generic conversion functions for base 2, 8, 10, and 16 data, to and
//from strings. Must support all int types, including signed. Include support for both
//string and wstring targets.
//We support the following notations for distinguishing non-base 10 numbers:
//Base 16: 0x????, 0X????, x????, X????, ????x, ????X, 0h????, 0H????, h????, H????, ????h, ????H
//Base 8:  0o????, 0O????, o????, O????, ????o, ????O, 0q????, 0Q????, q????, Q????, ????q, ????Q
//Base 2:  0b????, 0B????, b????, B????, ????b, ????B
//There is some debate surrounding negative numbers for bases other than 10. Should we
//support them? I think the answer to that is yes, in particular for base 16 numbers. How
//then do we represent them, since we're already prefixing the numbers? It would seem we
//should keep the minus sign at the start, eg, -0x200. Our parser will need to take into
//account the possibility of a minus sign before checking for a base prefix then.
char HexNybbleToChar(unsigned int nybble);
wchar_t HexNybbleToWChar(unsigned int nybble);
unsigned int HexCharToNybble(char nybble);
unsigned int HexWCharToNybble(wchar_t nybble);
template<class T> void IntToString(T& data, std::wstring& string);
template<class T> void IntToStringBase16(T data, std::wstring& string, unsigned int charLength = 0, bool includePrefix = true);
template<class T> void IntToStringBase10(T data, std::wstring& string);
template<class T> void IntToStringBase8(T data, std::wstring& string, bool includePrefix = true);
template<class T> void IntToStringBase2(T data, std::wstring& string, unsigned int charLength = 0, bool includePrefix = true);
template<class T> void StringToInt(const std::wstring& string, T& data);
template<class T> void StringToIntBase16(const std::wstring& string, T& data);
template<class T> void StringToIntBase10(const std::wstring& string, T& data);
template<class T> void StringToIntBase8(const std::wstring& string, T& data);
template<class T> void StringToIntBase2(const std::wstring& string, T& data);

std::vector<std::string> TokenizeString(const std::string& str, const std::string& delim);
std::vector<std::wstring> TokenizeString(const std::wstring& str, const std::wstring& delim);

#include "DataConversion.inl"
#endif
