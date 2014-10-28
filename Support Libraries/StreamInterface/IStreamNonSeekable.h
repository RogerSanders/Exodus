#ifndef __ISTREAMNONSEEKABLE_H__
#define __ISTREAMNONSEEKABLE_H__
#include <string>
#include <vector>
namespace Stream {

class IStreamNonSeekable
{
public:
	//Enumerations
	enum class ByteOrder;
	enum class TextEncoding;
	enum class NewLineEncoding;

	//Structures
	struct UnicodeCodePoint;

	//Typedefs
	typedef long long SizeType;

public:
	//Constructors
	virtual ~IStreamNonSeekable() = 0 {}

	//Byte order mark functions
	virtual bool ProcessByteOrderMark() = 0;
	virtual void InsertByteOrderMark() = 0;

	//Text encoding functions
	virtual TextEncoding GetTextEncoding() const = 0;
	virtual void SetTextEncoding(TextEncoding atextEncoding) = 0;

	//New line encoding functions
	virtual NewLineEncoding GetNewLineEncoding() const = 0;
	virtual void SetNewLineEncoding(NewLineEncoding anewLineEncoding) = 0;

	//Byte order functions
	virtual ByteOrder GetByteOrder() const = 0;
	virtual void SetByteOrder(ByteOrder abyteOrder) = 0;

	//Stream position
	virtual bool IsAtEnd() const = 0;
	virtual bool SkipBytes(SizeType byteCount) = 0;

	//Text char read functions
	//These functions read in text with newline and text format translation. Designed for
	//use on plain text files.
	inline bool ReadChar(ByteOrder abyteOrder, UnicodeCodePoint& data);
	inline bool ReadCharAsASCII(ByteOrder abyteOrder, UnicodeCodePoint& data);
	inline bool ReadCharAsUTF8(ByteOrder abyteOrder, UnicodeCodePoint& data);
	inline bool ReadCharAsUTF16(ByteOrder abyteOrder, UnicodeCodePoint& data);
	inline bool ReadCharAsUTF32(ByteOrder abyteOrder, UnicodeCodePoint& data);
	virtual bool ReadChar(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharAsASCII(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharAsUTF8(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharAsUTF16(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharAsUTF32(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharBigEndian(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharBigEndianAsASCII(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharBigEndianAsUTF8(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharBigEndianAsUTF16(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharBigEndianAsUTF32(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharLittleEndian(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharLittleEndianAsASCII(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharLittleEndianAsUTF8(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharLittleEndianAsUTF16(UnicodeCodePoint& data) = 0;
	virtual bool ReadCharLittleEndianAsUTF32(UnicodeCodePoint& data) = 0;

	//Fixed length text buffer read functions
	//These functions are designed to read in text from a fixed size field, such as one
	//which might be found in a binary file. Newline and text format translation are not
	//performed with these functions. The paddingChar parameter specifies a terminator
	//character for the string when performing the read. Once this character is
	//encountered when reading in the text buffer from the file, the remaining data in the
	//string will be discarded. The read string will always be null-terminated in memory.
	//Note that it is not an error for the string to not be terminated in the file source.
	//The read string will always be stored as null terminated in memory, so the
	//destination buffer needs to be at least length+1 code units in size.
	inline bool ReadTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0') = 0;
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0') = 0;
	inline bool ReadTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar = '\0');
	inline bool ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar = L'\0');

	//Data read functions
	inline bool ReadData(ByteOrder abyteOrder, bool& data);
	inline bool ReadData(ByteOrder abyteOrder, char& data);
	inline bool ReadData(ByteOrder abyteOrder, signed char& data);
	inline bool ReadData(ByteOrder abyteOrder, unsigned char& data);
	inline bool ReadData(ByteOrder abyteOrder, wchar_t& data);
	inline bool ReadData(ByteOrder abyteOrder, short& data);
	inline bool ReadData(ByteOrder abyteOrder, unsigned short& data);
	inline bool ReadData(ByteOrder abyteOrder, int& data);
	inline bool ReadData(ByteOrder abyteOrder, unsigned int& data);
	inline bool ReadData(ByteOrder abyteOrder, long& data);
	inline bool ReadData(ByteOrder abyteOrder, unsigned long& data);
	inline bool ReadData(ByteOrder abyteOrder, long long& data);
	inline bool ReadData(ByteOrder abyteOrder, unsigned long long& data);
	inline bool ReadData(ByteOrder abyteOrder, float& data);
	inline bool ReadData(ByteOrder abyteOrder, double& data);
	inline bool ReadData(ByteOrder abyteOrder, long double& data);
	virtual bool ReadData(bool& data) = 0;
	virtual bool ReadData(char& data) = 0;
	virtual bool ReadData(signed char& data) = 0;
	virtual bool ReadData(unsigned char& data) = 0;
	virtual bool ReadData(wchar_t& data) = 0;
	virtual bool ReadData(short& data) = 0;
	virtual bool ReadData(unsigned short& data) = 0;
	virtual bool ReadData(int& data) = 0;
	virtual bool ReadData(unsigned int& data) = 0;
	virtual bool ReadData(long& data) = 0;
	virtual bool ReadData(unsigned long& data) = 0;
	virtual bool ReadData(long long& data) = 0;
	virtual bool ReadData(unsigned long long& data) = 0;
	virtual bool ReadData(float& data) = 0;
	virtual bool ReadData(double& data) = 0;
	virtual bool ReadData(long double& data) = 0;
	virtual bool ReadDataBigEndian(bool& data) = 0;
	virtual bool ReadDataBigEndian(char& data) = 0;
	virtual bool ReadDataBigEndian(signed char& data) = 0;
	virtual bool ReadDataBigEndian(unsigned char& data) = 0;
	virtual bool ReadDataBigEndian(wchar_t& data) = 0;
	virtual bool ReadDataBigEndian(short& data) = 0;
	virtual bool ReadDataBigEndian(unsigned short& data) = 0;
	virtual bool ReadDataBigEndian(int& data) = 0;
	virtual bool ReadDataBigEndian(unsigned int& data) = 0;
	virtual bool ReadDataBigEndian(long& data) = 0;
	virtual bool ReadDataBigEndian(unsigned long& data) = 0;
	virtual bool ReadDataBigEndian(long long& data) = 0;
	virtual bool ReadDataBigEndian(unsigned long long& data) = 0;
	virtual bool ReadDataBigEndian(float& data) = 0;
	virtual bool ReadDataBigEndian(double& data) = 0;
	virtual bool ReadDataBigEndian(long double& data) = 0;
	virtual bool ReadDataLittleEndian(bool& data) = 0;
	virtual bool ReadDataLittleEndian(char& data) = 0;
	virtual bool ReadDataLittleEndian(signed char& data) = 0;
	virtual bool ReadDataLittleEndian(unsigned char& data) = 0;
	virtual bool ReadDataLittleEndian(wchar_t& data) = 0;
	virtual bool ReadDataLittleEndian(short& data) = 0;
	virtual bool ReadDataLittleEndian(unsigned short& data) = 0;
	virtual bool ReadDataLittleEndian(int& data) = 0;
	virtual bool ReadDataLittleEndian(unsigned int& data) = 0;
	virtual bool ReadDataLittleEndian(long& data) = 0;
	virtual bool ReadDataLittleEndian(unsigned long& data) = 0;
	virtual bool ReadDataLittleEndian(long long& data) = 0;
	virtual bool ReadDataLittleEndian(unsigned long long& data) = 0;
	virtual bool ReadDataLittleEndian(float& data) = 0;
	virtual bool ReadDataLittleEndian(double& data) = 0;
	virtual bool ReadDataLittleEndian(long double& data) = 0;

	//Array read functions
	inline bool ReadData(ByteOrder abyteOrder, bool* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, char* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, signed char* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, unsigned char* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, wchar_t* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, short* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, unsigned short* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, int* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, unsigned int* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, long* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, unsigned long* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, long long* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, unsigned long long* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, float* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, double* data, SizeType length);
	inline bool ReadData(ByteOrder abyteOrder, long double* data, SizeType length);
	virtual bool ReadData(bool* data, SizeType length) = 0;
	virtual bool ReadData(char* data, SizeType length) = 0;
	virtual bool ReadData(signed char* data, SizeType length) = 0;
	virtual bool ReadData(unsigned char* data, SizeType length) = 0;
	virtual bool ReadData(wchar_t* data, SizeType length) = 0;
	virtual bool ReadData(short* data, SizeType length) = 0;
	virtual bool ReadData(unsigned short* data, SizeType length) = 0;
	virtual bool ReadData(int* data, SizeType length) = 0;
	virtual bool ReadData(unsigned int* data, SizeType length) = 0;
	virtual bool ReadData(long* data, SizeType length) = 0;
	virtual bool ReadData(unsigned long* data, SizeType length) = 0;
	virtual bool ReadData(long long* data, SizeType length) = 0;
	virtual bool ReadData(unsigned long long* data, SizeType length) = 0;
	virtual bool ReadData(float* data, SizeType length) = 0;
	virtual bool ReadData(double* data, SizeType length) = 0;
	virtual bool ReadData(long double* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(bool* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(char* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(signed char* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(unsigned char* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(wchar_t* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(short* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(unsigned short* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(int* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(unsigned int* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(long* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(unsigned long* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(long long* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(unsigned long long* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(float* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(double* data, SizeType length) = 0;
	virtual bool ReadDataBigEndian(long double* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(bool* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(char* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(signed char* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(unsigned char* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(wchar_t* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(short* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(unsigned short* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(int* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(unsigned int* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(long* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(unsigned long* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(long long* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(unsigned long long* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(float* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(double* data, SizeType length) = 0;
	virtual bool ReadDataLittleEndian(long double* data, SizeType length) = 0;
	template<class T> inline bool ReadData(ByteOrder abyteOrder, std::vector<T>& data, SizeType length);
	template<class T> inline bool ReadData(std::vector<T>& data, SizeType length);
	template<class T> inline bool ReadDataBigEndian(std::vector<T>& data, SizeType length);
	template<class T> inline bool ReadDataLittleEndian(std::vector<T>& data, SizeType length);
	inline bool ReadData(std::vector<bool>& data, SizeType length);
	inline bool ReadDataBigEndian(std::vector<bool>& data, SizeType length);
	inline bool ReadDataLittleEndian(std::vector<bool>& data, SizeType length);

	//Text char write functions
	//These functions write text with newline and text format translation. Designed for
	//use on plain text files.
	inline bool WriteChar(ByteOrder abyteOrder, const UnicodeCodePoint& data);
	inline bool WriteCharAsASCII(ByteOrder abyteOrder, const UnicodeCodePoint& data);
	inline bool WriteCharAsUTF8(ByteOrder abyteOrder, const UnicodeCodePoint& data);
	inline bool WriteCharAsUTF16(ByteOrder abyteOrder, const UnicodeCodePoint& data);
	inline bool WriteCharAsUTF32(ByteOrder abyteOrder, const UnicodeCodePoint& data);
	virtual bool WriteChar(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharAsASCII(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharAsUTF8(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharAsUTF16(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharAsUTF32(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharBigEndian(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharBigEndianAsASCII(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharBigEndianAsUTF8(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharBigEndianAsUTF16(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharBigEndianAsUTF32(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharLittleEndian(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharLittleEndianAsASCII(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharLittleEndianAsUTF8(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharLittleEndianAsUTF16(const UnicodeCodePoint& data) = 0;
	virtual bool WriteCharLittleEndianAsUTF32(const UnicodeCodePoint& data) = 0;

	//Text string write functions
	//These functions write text with newline and text format translation. Designed for
	//use on plain text files.
	inline bool WriteText(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator = '\0');
	inline bool WriteText(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	inline bool WriteTextAsASCII(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator = '\0');
	inline bool WriteTextAsASCII(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	inline bool WriteTextAsUTF8(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator = '\0');
	inline bool WriteTextAsUTF8(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	inline bool WriteTextAsUTF16(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator = '\0');
	inline bool WriteTextAsUTF16(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	inline bool WriteTextAsUTF32(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator = '\0');
	inline bool WriteTextAsUTF32(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteText(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteText(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextAsASCII(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextAsUTF8(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextAsUTF16(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextAsUTF32(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextBigEndian(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextBigEndian(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextBigEndianAsASCII(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextBigEndianAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextBigEndianAsUTF8(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextBigEndianAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextBigEndianAsUTF16(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextBigEndianAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextBigEndianAsUTF32(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextBigEndianAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextLittleEndian(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextLittleEndian(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextLittleEndianAsASCII(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextLittleEndianAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextLittleEndianAsUTF8(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextLittleEndianAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextLittleEndianAsUTF16(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextLittleEndianAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	virtual bool WriteTextLittleEndianAsUTF32(const char* data, SizeType bufferSize, char terminator = '\0') = 0;
	virtual bool WriteTextLittleEndianAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0') = 0;
	inline bool WriteText(ByteOrder abyteOrder, const std::string& data);
	inline bool WriteText(ByteOrder abyteOrder, const std::wstring& data);
	inline bool WriteTextAsASCII(ByteOrder abyteOrder, const std::string& data);
	inline bool WriteTextAsASCII(ByteOrder abyteOrder, const std::wstring& data);
	inline bool WriteTextAsUTF8(ByteOrder abyteOrder, const std::string& data);
	inline bool WriteTextAsUTF8(ByteOrder abyteOrder, const std::wstring& data);
	inline bool WriteTextAsUTF16(ByteOrder abyteOrder, const std::string& data);
	inline bool WriteTextAsUTF16(ByteOrder abyteOrder, const std::wstring& data);
	inline bool WriteTextAsUTF32(ByteOrder abyteOrder, const std::string& data);
	inline bool WriteTextAsUTF32(ByteOrder abyteOrder, const std::wstring& data);
	inline bool WriteText(const std::string& data);
	inline bool WriteText(const std::wstring& data);
	inline bool WriteTextAsASCII(const std::string& data);
	inline bool WriteTextAsASCII(const std::wstring& data);
	inline bool WriteTextAsUTF8(const std::string& data);
	inline bool WriteTextAsUTF8(const std::wstring& data);
	inline bool WriteTextAsUTF16(const std::string& data);
	inline bool WriteTextAsUTF16(const std::wstring& data);
	inline bool WriteTextAsUTF32(const std::string& data);
	inline bool WriteTextAsUTF32(const std::wstring& data);
	inline bool WriteTextBigEndian(const std::string& data);
	inline bool WriteTextBigEndian(const std::wstring& data);
	inline bool WriteTextBigEndianAsASCII(const std::string& data);
	inline bool WriteTextBigEndianAsASCII(const std::wstring& data);
	inline bool WriteTextBigEndianAsUTF8(const std::string& data);
	inline bool WriteTextBigEndianAsUTF8(const std::wstring& data);
	inline bool WriteTextBigEndianAsUTF16(const std::string& data);
	inline bool WriteTextBigEndianAsUTF16(const std::wstring& data);
	inline bool WriteTextBigEndianAsUTF32(const std::string& data);
	inline bool WriteTextBigEndianAsUTF32(const std::wstring& data);
	inline bool WriteTextLittleEndian(const std::string& data);
	inline bool WriteTextLittleEndian(const std::wstring& data);
	inline bool WriteTextLittleEndianAsASCII(const std::string& data);
	inline bool WriteTextLittleEndianAsASCII(const std::wstring& data);
	inline bool WriteTextLittleEndianAsUTF8(const std::string& data);
	inline bool WriteTextLittleEndianAsUTF8(const std::wstring& data);
	inline bool WriteTextLittleEndianAsUTF16(const std::string& data);
	inline bool WriteTextLittleEndianAsUTF16(const std::wstring& data);
	inline bool WriteTextLittleEndianAsUTF32(const std::string& data);
	inline bool WriteTextLittleEndianAsUTF32(const std::wstring& data);

	//Fixed length text buffer write functions
	inline bool WriteTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0') = 0;
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0') = 0;
	inline bool WriteTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar = '\0');
	inline bool WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar = L'\0');

	//Data write functions
	inline bool WriteData(ByteOrder abyteOrder, bool data);
	inline bool WriteData(ByteOrder abyteOrder, char data);
	inline bool WriteData(ByteOrder abyteOrder, signed char data);
	inline bool WriteData(ByteOrder abyteOrder, unsigned char data);
	inline bool WriteData(ByteOrder abyteOrder, wchar_t data);
	inline bool WriteData(ByteOrder abyteOrder, short data);
	inline bool WriteData(ByteOrder abyteOrder, unsigned short data);
	inline bool WriteData(ByteOrder abyteOrder, int data);
	inline bool WriteData(ByteOrder abyteOrder, unsigned int data);
	inline bool WriteData(ByteOrder abyteOrder, long data);
	inline bool WriteData(ByteOrder abyteOrder, unsigned long data);
	inline bool WriteData(ByteOrder abyteOrder, long long data);
	inline bool WriteData(ByteOrder abyteOrder, unsigned long long data);
	inline bool WriteData(ByteOrder abyteOrder, float data);
	inline bool WriteData(ByteOrder abyteOrder, double data);
	inline bool WriteData(ByteOrder abyteOrder, long double data);
	virtual bool WriteData(bool data) = 0;
	virtual bool WriteData(char data) = 0;
	virtual bool WriteData(signed char data) = 0;
	virtual bool WriteData(unsigned char data) = 0;
	virtual bool WriteData(wchar_t data) = 0;
	virtual bool WriteData(short data) = 0;
	virtual bool WriteData(unsigned short data) = 0;
	virtual bool WriteData(int data) = 0;
	virtual bool WriteData(unsigned int data) = 0;
	virtual bool WriteData(long data) = 0;
	virtual bool WriteData(unsigned long data) = 0;
	virtual bool WriteData(long long data) = 0;
	virtual bool WriteData(unsigned long long data) = 0;
	virtual bool WriteData(float data) = 0;
	virtual bool WriteData(double data) = 0;
	virtual bool WriteData(long double data) = 0;
	virtual bool WriteDataBigEndian(bool data) = 0;
	virtual bool WriteDataBigEndian(char data) = 0;
	virtual bool WriteDataBigEndian(signed char data) = 0;
	virtual bool WriteDataBigEndian(unsigned char data) = 0;
	virtual bool WriteDataBigEndian(wchar_t data) = 0;
	virtual bool WriteDataBigEndian(short data) = 0;
	virtual bool WriteDataBigEndian(unsigned short data) = 0;
	virtual bool WriteDataBigEndian(int data) = 0;
	virtual bool WriteDataBigEndian(unsigned int data) = 0;
	virtual bool WriteDataBigEndian(long data) = 0;
	virtual bool WriteDataBigEndian(unsigned long data) = 0;
	virtual bool WriteDataBigEndian(long long data) = 0;
	virtual bool WriteDataBigEndian(unsigned long long data) = 0;
	virtual bool WriteDataBigEndian(float data) = 0;
	virtual bool WriteDataBigEndian(double data) = 0;
	virtual bool WriteDataBigEndian(long double data) = 0;
	virtual bool WriteDataLittleEndian(bool data) = 0;
	virtual bool WriteDataLittleEndian(char data) = 0;
	virtual bool WriteDataLittleEndian(signed char data) = 0;
	virtual bool WriteDataLittleEndian(unsigned char data) = 0;
	virtual bool WriteDataLittleEndian(wchar_t data) = 0;
	virtual bool WriteDataLittleEndian(short data) = 0;
	virtual bool WriteDataLittleEndian(unsigned short data) = 0;
	virtual bool WriteDataLittleEndian(int data) = 0;
	virtual bool WriteDataLittleEndian(unsigned int data) = 0;
	virtual bool WriteDataLittleEndian(long data) = 0;
	virtual bool WriteDataLittleEndian(unsigned long data) = 0;
	virtual bool WriteDataLittleEndian(long long data) = 0;
	virtual bool WriteDataLittleEndian(unsigned long long data) = 0;
	virtual bool WriteDataLittleEndian(float data) = 0;
	virtual bool WriteDataLittleEndian(double data) = 0;
	virtual bool WriteDataLittleEndian(long double data) = 0;

	//Array write functions
	inline bool WriteData(ByteOrder abyteOrder, const bool* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const char* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const signed char* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const unsigned char* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const wchar_t* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const short* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const unsigned short* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const int* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const unsigned int* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const long* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const unsigned long* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const long long* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const unsigned long long* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const float* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const double* data, SizeType length);
	inline bool WriteData(ByteOrder abyteOrder, const long double* data, SizeType length);
	virtual bool WriteData(const bool* data, SizeType length) = 0;
	virtual bool WriteData(const char* data, SizeType length) = 0;
	virtual bool WriteData(const signed char* data, SizeType length) = 0;
	virtual bool WriteData(const unsigned char* data, SizeType length) = 0;
	virtual bool WriteData(const wchar_t* data, SizeType length) = 0;
	virtual bool WriteData(const short* data, SizeType length) = 0;
	virtual bool WriteData(const unsigned short* data, SizeType length) = 0;
	virtual bool WriteData(const int* data, SizeType length) = 0;
	virtual bool WriteData(const unsigned int* data, SizeType length) = 0;
	virtual bool WriteData(const long* data, SizeType length) = 0;
	virtual bool WriteData(const unsigned long* data, SizeType length) = 0;
	virtual bool WriteData(const long long* data, SizeType length) = 0;
	virtual bool WriteData(const unsigned long long* data, SizeType length) = 0;
	virtual bool WriteData(const float* data, SizeType length) = 0;
	virtual bool WriteData(const double* data, SizeType length) = 0;
	virtual bool WriteData(const long double* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const bool* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const char* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const signed char* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const unsigned char* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const wchar_t* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const short* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const unsigned short* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const int* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const unsigned int* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const long* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const unsigned long* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const long long* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const unsigned long long* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const float* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const double* data, SizeType length) = 0;
	virtual bool WriteDataBigEndian(const long double* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const bool* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const char* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const signed char* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const unsigned char* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const wchar_t* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const short* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const unsigned short* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const int* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const unsigned int* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const long* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const unsigned long* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const long long* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const unsigned long long* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const float* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const double* data, SizeType length) = 0;
	virtual bool WriteDataLittleEndian(const long double* data, SizeType length) = 0;
	template<class T> inline bool WriteData(ByteOrder abyteOrder, const std::vector<T>& data);
	template<class T> inline bool WriteData(const std::vector<T>& data);
	template<class T> inline bool WriteDataBigEndian(const std::vector<T>& data);
	template<class T> inline bool WriteDataLittleEndian(const std::vector<T>& data);
	inline bool WriteData(const std::vector<bool>& data);
	inline bool WriteDataBigEndian(const std::vector<bool>& data);
	inline bool WriteDataLittleEndian(const std::vector<bool>& data);
	template<class T> inline bool WriteDataMultipleTimes(ByteOrder abyteOrder, T data, SizeType count);
	template<class T> inline bool WriteDataMultipleTimes(T data, SizeType count);
	template<class T> inline bool WriteDataBigEndianMultipleTimes(T data, SizeType count);
	template<class T> inline bool WriteDataLittleEndianMultipleTimes(T data, SizeType count);
};

} //Close namespace Stream
#include "IStreamNonSeekable.inl"
#endif
