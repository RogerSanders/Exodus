/*--------------------------------------------------------------------------------------*\
Things to do:
-Apparently, wchar_t is 32-bit (UTF32 encoding) on unix. Make this library fully portable,
so that it can work regardless of the particular encoding wchar_t uses.
\*--------------------------------------------------------------------------------------*/
#include "StreamInterface/StreamInterface.pkg"
#ifndef __STREAM_H__
#define __STREAM_H__
namespace Stream {

//##TODO## Detect byte order and new line encoding for this platform
//#include <boost/detail/endian.hpp>
//#define STREAM_PLATFORMBYTEORDER_BIGENDIAN BOOST_BIG_ENDIAN

#define STREAM_PLATFORMBYTEORDER_BIGENDIAN 0
#define STREAM_PLATFORMNEWLINEENCODING DOS

template<class B> class Stream :public B
{
public:
	//Constructors
	inline Stream(typename B::TextEncoding atextEncoding = B::TextEncoding::UTF8, typename B::NewLineEncoding anewLineEncoding = B::NewLineEncoding::STREAM_PLATFORMNEWLINEENCODING, typename B::ByteOrder abyteOrder = B::ByteOrder::Platform);

	//Byte order mark functions
	virtual bool ProcessByteOrderMark();
	virtual void InsertByteOrderMark();

	//Text encoding functions
	virtual typename B::TextEncoding GetTextEncoding() const;
	virtual void SetTextEncoding(typename B::TextEncoding atextEncoding);

	//New line encoding functions
	virtual typename B::NewLineEncoding GetNewLineEncoding() const;
	virtual void SetNewLineEncoding(typename B::NewLineEncoding anewLineEncoding);

	//Byte order functions
	virtual typename B::ByteOrder GetByteOrder() const;
	virtual void SetByteOrder(typename B::ByteOrder abyteOrder);

	//Text char read functions
	//These functions read in text with newline and text format translation. Designed for
	//use on plain text files.
	virtual bool ReadChar(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharAsASCII(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharAsUTF8(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharAsUTF16(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharAsUTF32(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharBigEndian(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsASCII(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsUTF8(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsUTF16(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsUTF32(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndian(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsASCII(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsUTF8(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsUTF16(typename B::UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsUTF32(typename B::UnicodeCodePoint& data);

	//Fixed length text buffer read functions
	using B::ReadTextFixedLengthBufferAsASCII;
	using B::ReadTextFixedLengthBufferAsUTF8;
	using B::ReadTextFixedLengthBufferAsUTF16;
	using B::ReadTextFixedLengthBufferAsUTF32;
	using B::ReadTextBigEndianFixedLengthBufferAsASCII;
	using B::ReadTextBigEndianFixedLengthBufferAsUTF8;
	using B::ReadTextBigEndianFixedLengthBufferAsUTF16;
	using B::ReadTextBigEndianFixedLengthBufferAsUTF32;
	using B::ReadTextLittleEndianFixedLengthBufferAsASCII;
	using B::ReadTextLittleEndianFixedLengthBufferAsUTF8;
	using B::ReadTextLittleEndianFixedLengthBufferAsUTF16;
	using B::ReadTextLittleEndianFixedLengthBufferAsUTF32;
	virtual bool ReadTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');

	//Data read functions
	using B::ReadData;
	using B::ReadDataBigEndian;
	using B::ReadDataLittleEndian;
	virtual bool ReadData(bool& data);
	virtual bool ReadData(char& data);
	virtual bool ReadData(signed char& data);
	virtual bool ReadData(unsigned char& data);
	virtual bool ReadData(wchar_t& data);
	virtual bool ReadData(short& data);
	virtual bool ReadData(unsigned short& data);
	virtual bool ReadData(int& data);
	virtual bool ReadData(unsigned int& data);
	virtual bool ReadData(long& data);
	virtual bool ReadData(unsigned long& data);
	virtual bool ReadData(long long& data);
	virtual bool ReadData(unsigned long long& data);
	virtual bool ReadData(float& data);
	virtual bool ReadData(double& data);
	virtual bool ReadData(long double& data);
	virtual bool ReadDataBigEndian(bool& data);
	virtual bool ReadDataBigEndian(char& data);
	virtual bool ReadDataBigEndian(signed char& data);
	virtual bool ReadDataBigEndian(unsigned char& data);
	virtual bool ReadDataBigEndian(wchar_t& data);
	virtual bool ReadDataBigEndian(short& data);
	virtual bool ReadDataBigEndian(unsigned short& data);
	virtual bool ReadDataBigEndian(int& data);
	virtual bool ReadDataBigEndian(unsigned int& data);
	virtual bool ReadDataBigEndian(long& data);
	virtual bool ReadDataBigEndian(unsigned long& data);
	virtual bool ReadDataBigEndian(long long& data);
	virtual bool ReadDataBigEndian(unsigned long long& data);
	virtual bool ReadDataBigEndian(float& data);
	virtual bool ReadDataBigEndian(double& data);
	virtual bool ReadDataBigEndian(long double& data);
	virtual bool ReadDataLittleEndian(bool& data);
	virtual bool ReadDataLittleEndian(char& data);
	virtual bool ReadDataLittleEndian(signed char& data);
	virtual bool ReadDataLittleEndian(unsigned char& data);
	virtual bool ReadDataLittleEndian(wchar_t& data);
	virtual bool ReadDataLittleEndian(short& data);
	virtual bool ReadDataLittleEndian(unsigned short& data);
	virtual bool ReadDataLittleEndian(int& data);
	virtual bool ReadDataLittleEndian(unsigned int& data);
	virtual bool ReadDataLittleEndian(long& data);
	virtual bool ReadDataLittleEndian(unsigned long& data);
	virtual bool ReadDataLittleEndian(long long& data);
	virtual bool ReadDataLittleEndian(unsigned long long& data);
	virtual bool ReadDataLittleEndian(float& data);
	virtual bool ReadDataLittleEndian(double& data);
	virtual bool ReadDataLittleEndian(long double& data);

	//Array read functions
	virtual bool ReadData(bool* data, typename B::SizeType length);
	virtual bool ReadData(char* data, typename B::SizeType length);
	virtual bool ReadData(signed char* data, typename B::SizeType length);
	virtual bool ReadData(unsigned char* data, typename B::SizeType length);
	virtual bool ReadData(wchar_t* data, typename B::SizeType length);
	virtual bool ReadData(short* data, typename B::SizeType length);
	virtual bool ReadData(unsigned short* data, typename B::SizeType length);
	virtual bool ReadData(int* data, typename B::SizeType length);
	virtual bool ReadData(unsigned int* data, typename B::SizeType length);
	virtual bool ReadData(long* data, typename B::SizeType length);
	virtual bool ReadData(unsigned long* data, typename B::SizeType length);
	virtual bool ReadData(long long* data, typename B::SizeType length);
	virtual bool ReadData(unsigned long long* data, typename B::SizeType length);
	virtual bool ReadData(float* data, typename B::SizeType length);
	virtual bool ReadData(double* data, typename B::SizeType length);
	virtual bool ReadData(long double* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(bool* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(char* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(signed char* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(unsigned char* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(wchar_t* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(short* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(unsigned short* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(int* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(unsigned int* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(long* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(unsigned long* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(long long* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(unsigned long long* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(float* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(double* data, typename B::SizeType length);
	virtual bool ReadDataBigEndian(long double* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(bool* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(char* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(signed char* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(unsigned char* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(wchar_t* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(short* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(unsigned short* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(int* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(unsigned int* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(long* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(unsigned long* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(long long* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(unsigned long long* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(float* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(double* data, typename B::SizeType length);
	virtual bool ReadDataLittleEndian(long double* data, typename B::SizeType length);

	//Text char write functions
	virtual bool WriteChar(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharAsASCII(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharAsUTF8(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharAsUTF16(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharAsUTF32(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharBigEndian(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsASCII(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsUTF8(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsUTF16(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsUTF32(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndian(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsASCII(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsUTF8(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsUTF16(const typename B::UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsUTF32(const typename B::UnicodeCodePoint& data);

	//Text string write functions
	using B::WriteText;
	virtual bool WriteText(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteText(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsASCII(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsASCII(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsUTF8(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsUTF8(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsUTF16(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsUTF16(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsUTF32(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsUTF32(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndian(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndian(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsASCII(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsASCII(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsUTF8(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsUTF8(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsUTF16(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsUTF16(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsUTF32(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsUTF32(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndian(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndian(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsASCII(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsASCII(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsUTF8(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsUTF8(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsUTF16(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsUTF16(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsUTF32(const char* data, typename B::SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsUTF32(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator = L'\0');

	//Fixed length buffer text write functions
	using B::WriteTextFixedLengthBufferAsASCII;
	using B::WriteTextFixedLengthBufferAsUTF8;
	using B::WriteTextFixedLengthBufferAsUTF16;
	using B::WriteTextFixedLengthBufferAsUTF32;
	using B::WriteTextBigEndianFixedLengthBufferAsASCII;
	using B::WriteTextBigEndianFixedLengthBufferAsUTF8;
	using B::WriteTextBigEndianFixedLengthBufferAsUTF16;
	using B::WriteTextBigEndianFixedLengthBufferAsUTF32;
	using B::WriteTextLittleEndianFixedLengthBufferAsASCII;
	using B::WriteTextLittleEndianFixedLengthBufferAsUTF8;
	using B::WriteTextLittleEndianFixedLengthBufferAsUTF16;
	using B::WriteTextLittleEndianFixedLengthBufferAsUTF32;
	virtual bool WriteTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');

	//Data write functions
	using B::WriteData;
	using B::WriteDataBigEndian;
	using B::WriteDataLittleEndian;
	virtual bool WriteData(bool data);
	virtual bool WriteData(char data);
	virtual bool WriteData(signed char data);
	virtual bool WriteData(unsigned char data);
	virtual bool WriteData(wchar_t data);
	virtual bool WriteData(short data);
	virtual bool WriteData(unsigned short data);
	virtual bool WriteData(int data);
	virtual bool WriteData(unsigned int data);
	virtual bool WriteData(long data);
	virtual bool WriteData(unsigned long data);
	virtual bool WriteData(long long data);
	virtual bool WriteData(unsigned long long data);
	virtual bool WriteData(float data);
	virtual bool WriteData(double data);
	virtual bool WriteData(long double data);
	virtual bool WriteDataBigEndian(bool data);
	virtual bool WriteDataBigEndian(char data);
	virtual bool WriteDataBigEndian(signed char data);
	virtual bool WriteDataBigEndian(unsigned char data);
	virtual bool WriteDataBigEndian(wchar_t data);
	virtual bool WriteDataBigEndian(short data);
	virtual bool WriteDataBigEndian(unsigned short data);
	virtual bool WriteDataBigEndian(int data);
	virtual bool WriteDataBigEndian(unsigned int data);
	virtual bool WriteDataBigEndian(long data);
	virtual bool WriteDataBigEndian(unsigned long data);
	virtual bool WriteDataBigEndian(long long data);
	virtual bool WriteDataBigEndian(unsigned long long data);
	virtual bool WriteDataBigEndian(float data);
	virtual bool WriteDataBigEndian(double data);
	virtual bool WriteDataBigEndian(long double data);
	virtual bool WriteDataLittleEndian(bool data);
	virtual bool WriteDataLittleEndian(char data);
	virtual bool WriteDataLittleEndian(signed char data);
	virtual bool WriteDataLittleEndian(unsigned char data);
	virtual bool WriteDataLittleEndian(wchar_t data);
	virtual bool WriteDataLittleEndian(short data);
	virtual bool WriteDataLittleEndian(unsigned short data);
	virtual bool WriteDataLittleEndian(int data);
	virtual bool WriteDataLittleEndian(unsigned int data);
	virtual bool WriteDataLittleEndian(long data);
	virtual bool WriteDataLittleEndian(unsigned long data);
	virtual bool WriteDataLittleEndian(long long data);
	virtual bool WriteDataLittleEndian(unsigned long long data);
	virtual bool WriteDataLittleEndian(float data);
	virtual bool WriteDataLittleEndian(double data);
	virtual bool WriteDataLittleEndian(long double data);

	//Array write functions
	virtual bool WriteData(const bool* data, typename B::SizeType length);
	virtual bool WriteData(const char* data, typename B::SizeType length);
	virtual bool WriteData(const signed char* data, typename B::SizeType length);
	virtual bool WriteData(const unsigned char* data, typename B::SizeType length);
	virtual bool WriteData(const wchar_t* data, typename B::SizeType length);
	virtual bool WriteData(const short* data, typename B::SizeType length);
	virtual bool WriteData(const unsigned short* data, typename B::SizeType length);
	virtual bool WriteData(const int* data, typename B::SizeType length);
	virtual bool WriteData(const unsigned int* data, typename B::SizeType length);
	virtual bool WriteData(const long* data, typename B::SizeType length);
	virtual bool WriteData(const unsigned long* data, typename B::SizeType length);
	virtual bool WriteData(const long long* data, typename B::SizeType length);
	virtual bool WriteData(const unsigned long long* data, typename B::SizeType length);
	virtual bool WriteData(const float* data, typename B::SizeType length);
	virtual bool WriteData(const double* data, typename B::SizeType length);
	virtual bool WriteData(const long double* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const bool* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const char* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const signed char* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const unsigned char* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const wchar_t* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const short* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const unsigned short* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const int* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const unsigned int* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const long* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const unsigned long* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const long long* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const unsigned long long* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const float* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const double* data, typename B::SizeType length);
	virtual bool WriteDataBigEndian(const long double* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const bool* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const char* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const signed char* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned char* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const wchar_t* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const short* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned short* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const int* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned int* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const long* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned long* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const long long* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned long long* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const float* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const double* data, typename B::SizeType length);
	virtual bool WriteDataLittleEndian(const long double* data, typename B::SizeType length);

protected:
	//Native byte order read functions
	virtual bool ReadBinaryNativeByteOrder(char& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(signed char& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned char& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(wchar_t& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(short& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned short& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(int& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned int& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(long& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned long& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(long long& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned long long& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(float& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(double& data) = 0;
	virtual bool ReadBinaryNativeByteOrder(long double& data) = 0;

	//Native byte order array read functions
	virtual bool ReadBinaryNativeByteOrder(char* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(signed char* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned char* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(wchar_t* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(short* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned short* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(int* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned int* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(long* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned long* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(long long* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned long long* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(float* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(double* data, typename B::SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(long double* data, typename B::SizeType length) = 0;

	//Native byte order write functions
	virtual bool WriteBinaryNativeByteOrder(char data) = 0;
	virtual bool WriteBinaryNativeByteOrder(signed char data) = 0;
	virtual bool WriteBinaryNativeByteOrder(unsigned char data) = 0;
	virtual bool WriteBinaryNativeByteOrder(wchar_t data) = 0;
	virtual bool WriteBinaryNativeByteOrder(short data) = 0;
	virtual bool WriteBinaryNativeByteOrder(unsigned short data) = 0;
	virtual bool WriteBinaryNativeByteOrder(int data) = 0;
	virtual bool WriteBinaryNativeByteOrder(unsigned int data) = 0;
	virtual bool WriteBinaryNativeByteOrder(long data) = 0;
	virtual bool WriteBinaryNativeByteOrder(unsigned long data) = 0;
	virtual bool WriteBinaryNativeByteOrder(long long data) = 0;
	virtual bool WriteBinaryNativeByteOrder(unsigned long long data) = 0;
	virtual bool WriteBinaryNativeByteOrder(float data) = 0;
	virtual bool WriteBinaryNativeByteOrder(double data) = 0;
	virtual bool WriteBinaryNativeByteOrder(long double data) = 0;

	//Native byte order array write functions
	virtual bool WriteBinaryNativeByteOrder(const char* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const signed char* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned char* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const wchar_t* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const short* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned short* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const int* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned int* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const long* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned long* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const long long* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned long long* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const float* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const double* data, typename B::SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const long double* data, typename B::SizeType length) = 0;

private:
	//Internal text format independent char read functions
	inline bool ReadCharInternal(typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsASCII(typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsUTF8(typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsUTF16(typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsUTF32(typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);

	//Internal fixed length text buffer read functions
	inline bool ReadTextInternalFixedLengthBufferAsASCII(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsASCII(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF8(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF8(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF16(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF16(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF32(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF32(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar);

	//Internal type-independent data read functions
	template<class T> bool ReadDataInternal(T& data, typename B::ByteOrder abyteOrder);
	template<class T> bool ReadDataInternal(T& data);
	template<class T> bool ReadDataInternalBigEndian(T& data);
	template<class T> bool ReadDataInternalLittleEndian(T& data);
	template<class T> bool ReadDataInternal(T* data, typename B::SizeType length, typename B::ByteOrder abyteOrder);
	template<class T> bool ReadDataInternal(T* data, typename B::SizeType length);
	template<class T> bool ReadDataInternalBigEndian(T* data, typename B::SizeType length);
	template<class T> bool ReadDataInternalLittleEndian(T* data, typename B::SizeType length);

	//Inverted byte order read functions
	template<class T> bool ReadBinaryInvertedByteOrder(T& data);
	template<class T> bool ReadBinaryInvertedByteOrder(T* data, typename B::SizeType length);

	//Internal text format independent char write functions
	inline bool WriteCharInternal(const typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsASCII(const typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsUTF8(const typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsUTF16(const typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsUTF32(const typename B::UnicodeCodePoint& data, typename B::ByteOrder abyteOrder, typename B::SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);

	//Text string write functions
	inline bool WriteTextInternal(typename B::ByteOrder abyteOrder, const char* data, typename B::SizeType bufferSize, char terminator);
	inline bool WriteTextInternal(typename B::ByteOrder abyteOrder, const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsASCII(typename B::ByteOrder abyteOrder, const char* data, typename B::SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsASCII(typename B::ByteOrder abyteOrder, const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsUTF8(typename B::ByteOrder abyteOrder, const char* data, typename B::SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsUTF8(typename B::ByteOrder abyteOrder, const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsUTF16(typename B::ByteOrder abyteOrder, const char* data, typename B::SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsUTF16(typename B::ByteOrder abyteOrder, const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsUTF32(typename B::ByteOrder abyteOrder, const char* data, typename B::SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsUTF32(typename B::ByteOrder abyteOrder, const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator);

	//Internal fixed length text buffer write functions
	inline bool WriteTextInternalFixedLengthBufferAsASCII(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsASCII(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF8(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF8(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF16(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF16(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF32(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF32(typename B::ByteOrder abyteOrder, typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar);

	//Internal type-independent data write functions
	template<class T> bool WriteDataInternal(T data, typename B::ByteOrder abyteOrder);
	template<class T> bool WriteDataInternal(T data);
	template<class T> bool WriteDataInternalBigEndian(T data);
	template<class T> bool WriteDataInternalLittleEndian(T data);
	template<class T> bool WriteDataInternal(const T* data, typename B::SizeType length, typename B::ByteOrder abyteOrder);
	template<class T> bool WriteDataInternal(const T* data, typename B::SizeType length);
	template<class T> bool WriteDataInternalBigEndian(const T* data, typename B::SizeType length);
	template<class T> bool WriteDataInternalLittleEndian(const T* data, typename B::SizeType length);

	//Inverted byte order write functions
	template<class T> bool WriteBinaryInvertedByteOrder(T data);
	template<class T> bool WriteBinaryInvertedByteOrder(const T* data, typename B::SizeType length);

	//Text conversion functions
	static void ConvertASCIIToUnicodeCodePoint(unsigned char source, typename B::UnicodeCodePoint& target);
	static void ConvertUTF32ToUnicodeCodePoint(unsigned int source, typename B::UnicodeCodePoint& target);
	static void ConvertUTF16ToUnicodeCodePoint(unsigned short codeUnit1, unsigned short codeUnit2, bool surrogatePair, typename B::UnicodeCodePoint& target);
	static void ConvertUnicodeCodePointToUTF16(const typename B::UnicodeCodePoint& source, unsigned short& codeUnit1, unsigned short& codeUnit2, bool& surrogatePair);
	static void ConvertUnicodeCodePointToUTF32(const typename B::UnicodeCodePoint& source, unsigned int& target);
	static bool ConvertCharToUnicodeCodePoint(char source, typename B::UnicodeCodePoint& target);
	static bool ConvertWCharTToUnicodeCodePoint(const wchar_t* source, typename B::UnicodeCodePoint& target, typename B::SizeType bufferSize, typename B::SizeType& codeUnitsRead);
	static bool ConvertUnicodeCodePointToChar(const typename B::UnicodeCodePoint& source, char& target);
	static bool ConvertUnicodeCodePointToWCharT(const typename B::UnicodeCodePoint& source, wchar_t* target, typename B::SizeType bufferSize, typename B::SizeType& codeUnitsWritten);

	//Bool conversion functions
	inline static unsigned char BoolToByte(bool data);
	inline static bool ByteToBool(unsigned char data);

private:
	typename B::ByteOrder byteOrder;
	typename B::TextEncoding textEncoding;
	typename B::NewLineEncoding newLineEncoding;
};

} //Close namespace Stream
#include "Stream.inl"
#endif
