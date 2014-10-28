#ifndef __BINARYSTRING_H__
#define __BINARYSTRING_H__
#include <string>
#include <vector>

//##TODO## Split this into separate logical files
//##TODO## Group these together into a class or namespace
//##TODO## Make a file for string helper functions
//We support the following notations for distinguishing number bases:
//Base 16: 0x????, 0X????, 0h????, 0H????
//Base 10: 0d????, 0D????
//Base 8:  0o????, 0O????, 0q????, 0Q????
//Base 2:  0b????, 0B????
//-We don't support any postfix base encoding forms, because they can be ambiguous with
//unqualified number encodings from other bases. The 'b' and 'd' postfixes in particular
//would conflict with base 16 encoding, but other encoding schemes like base 32 encoding
//that could be added later would conflict with all postfixes.
//-Any unusual variations of base encoding should be supported through separate
//compatibility layers for specific platforms, IE, an interpretation layer for Intel or
//Motorola assembly numeric encoding. These strings could be passed through this kind of
//compatibility layer before processing, IE, by stripping off the platform specific base
//identifier, and feeding the modified string along with the detected base into our
//generic routines here.
inline char HexNybbleToChar(unsigned int nybble);
inline wchar_t HexNybbleToWChar(unsigned int nybble);
inline unsigned int HexCharToNybble(char nybble);
inline unsigned int HexWCharToNybble(wchar_t nybble);
template<class T> bool IntToString(T data, std::wstring& string, unsigned int stringBase = 10);
template<class T> void IntToStringBase16(T data, std::wstring& string, unsigned int charLength = 0, bool encodeType = true);
template<class T> void IntToStringBase10(T data, std::wstring& string, unsigned int charLength = 0, bool encodeType = false);
template<class T> void IntToStringBase8(T data, std::wstring& string, unsigned int charLength = 0, bool encodeType = true);
template<class T> void IntToStringBase2(T data, std::wstring& string, unsigned int charLength = 0, bool encodeType = true);
template<class T> bool StringToInt(const std::wstring& string, T& data, unsigned int defaultBase = 10);
template<class T> bool StringToIntBase16(const std::wstring& string, T& data);
template<class T> bool StringToIntBase10(const std::wstring& string, T& data);
template<class T> bool StringToIntBase8(const std::wstring& string, T& data);
template<class T> bool StringToIntBase2(const std::wstring& string, T& data);

template<class T> void PointerToString(T* pointer, std::wstring& string);

inline void BoolToString(bool data, std::wstring& string, bool numericForm = false);
inline bool StringToBool(const std::wstring& string, bool& data);
inline void FloatToString(float data, std::wstring& string, bool scientificNotation = false, unsigned int fractionalNumberCharLength = 0, unsigned int wholeNumberCharLength = 0);
inline bool StringToFloat(const std::wstring& string, float& data);
inline void DoubleToString(double data, std::wstring& string, bool scientificNotation = false, unsigned int fractionalNumberCharLength = 0, unsigned int wholeNumberCharLength = 0);
inline bool StringToDouble(const std::wstring& string, double& data);

inline std::vector<std::string> TokenizeString(const std::string& str, const std::string& delim);
inline std::vector<std::wstring> TokenizeString(const std::wstring& str, const std::wstring& delim);

inline char CharToLower(char data);
inline wchar_t CharToLower(wchar_t data);
inline char CharToUpper(char data);
inline wchar_t CharToUpper(wchar_t data);
inline std::string StringToLower(const std::string& string);
inline std::wstring StringToLower(const std::wstring& string);
inline std::string StringToUpper(const std::string& string);
inline std::wstring StringToUpper(const std::wstring& string);

inline std::wstring StringToWString(const std::string& string);
inline std::string WStringToString(const std::wstring& string);

#include "DataConversion.inl"
#endif
