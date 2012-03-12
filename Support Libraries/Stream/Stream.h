/*--------------------------------------------------------------------------------------*\
Things to do:
-Apparently, wchar_t is 32-bit (UTF32 encoding) on unix. Make this library fully portable,
so that it can work regardless of the particular encoding wchar_t uses.
-Create an abstract class above stream which contains the seeking interface. Allow a base
stream class which doesn't allow seeking, which non-seekable stream targets can derive
from. Functions which require seeking can ask for a seekable stream object by using the
abstract seekable base class, with a compile time error if a non-seekable stream is
provided.
\*--------------------------------------------------------------------------------------*/
#include "StreamInterface/StreamInterface.pkg"
#ifndef __STREAM_H__
#define __STREAM_H__
namespace Stream {

//##TODO## Detect byte order and new line encoding for this platform
//#include <boost/detail/endian.hpp>
//#define STREAM_PLATFORMBYTEORDER_BIGENDIAN BOOST_BIG_ENDIAN

#define STREAM_PLATFORMBYTEORDER_BIGENDIAN 0
#define STREAM_PLATFORMNEWLINEENCODING NEWLINEENCODING_DOS

class Stream :public IStream
{
public:
	//Constructors
	inline Stream(TextEncoding atextEncoding = TEXTENCODING_ASCII, NewLineEncoding anewLineEncoding = STREAM_PLATFORMNEWLINEENCODING, ByteOrder abyteOrder = BYTEORDER_PLATFORM);

	//Byte order mark functions
	virtual bool ProcessByteOrderMark();
	virtual void InsertByteOrderMark();

	//Text encoding functions
	virtual TextEncoding GetTextEncoding() const;
	virtual void SetTextEncoding(TextEncoding atextEncoding);

	//New line encoding functions
	virtual NewLineEncoding GetNewLineEncoding() const;
	virtual void SetNewLineEncoding(NewLineEncoding anewLineEncoding);

	//Byte order functions
	virtual ByteOrder GetByteOrder() const;
	virtual void SetByteOrder(ByteOrder abyteOrder);

	//Text char read functions
	//These functions read in text with newline and text format translation. Designed for
	//use on plain text files.
	virtual bool ReadChar(UnicodeCodePoint& data);
	virtual bool ReadCharAsASCII(UnicodeCodePoint& data);
	virtual bool ReadCharAsUTF8(UnicodeCodePoint& data);
	virtual bool ReadCharAsUTF16(UnicodeCodePoint& data);
	virtual bool ReadCharAsUTF32(UnicodeCodePoint& data);
	virtual bool ReadCharBigEndian(UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsASCII(UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsUTF8(UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsUTF16(UnicodeCodePoint& data);
	virtual bool ReadCharBigEndianAsUTF32(UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndian(UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsASCII(UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsUTF8(UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsUTF16(UnicodeCodePoint& data);
	virtual bool ReadCharLittleEndianAsUTF32(UnicodeCodePoint& data);

	//Fixed length text buffer read functions
	using IStream::ReadTextFixedLengthBufferAsASCII;
	using IStream::ReadTextFixedLengthBufferAsUTF8;
	using IStream::ReadTextFixedLengthBufferAsUTF16;
	using IStream::ReadTextFixedLengthBufferAsUTF32;
	using IStream::ReadTextBigEndianFixedLengthBufferAsASCII;
	using IStream::ReadTextBigEndianFixedLengthBufferAsUTF8;
	using IStream::ReadTextBigEndianFixedLengthBufferAsUTF16;
	using IStream::ReadTextBigEndianFixedLengthBufferAsUTF32;
	using IStream::ReadTextLittleEndianFixedLengthBufferAsASCII;
	using IStream::ReadTextLittleEndianFixedLengthBufferAsUTF8;
	using IStream::ReadTextLittleEndianFixedLengthBufferAsUTF16;
	using IStream::ReadTextLittleEndianFixedLengthBufferAsUTF32;
	virtual bool ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar = '\0');
	virtual bool ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar = L'\0');

	//Data read functions
	using IStream::ReadData;
	using IStream::ReadDataBigEndian;
	using IStream::ReadDataLittleEndian;
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
	virtual bool ReadData(bool* data, SizeType length);
	virtual bool ReadData(char* data, SizeType length);
	virtual bool ReadData(signed char* data, SizeType length);
	virtual bool ReadData(unsigned char* data, SizeType length);
	virtual bool ReadData(wchar_t* data, SizeType length);
	virtual bool ReadData(short* data, SizeType length);
	virtual bool ReadData(unsigned short* data, SizeType length);
	virtual bool ReadData(int* data, SizeType length);
	virtual bool ReadData(unsigned int* data, SizeType length);
	virtual bool ReadData(long* data, SizeType length);
	virtual bool ReadData(unsigned long* data, SizeType length);
	virtual bool ReadData(long long* data, SizeType length);
	virtual bool ReadData(unsigned long long* data, SizeType length);
	virtual bool ReadData(float* data, SizeType length);
	virtual bool ReadData(double* data, SizeType length);
	virtual bool ReadData(long double* data, SizeType length);
	virtual bool ReadDataBigEndian(bool* data, SizeType length);
	virtual bool ReadDataBigEndian(char* data, SizeType length);
	virtual bool ReadDataBigEndian(signed char* data, SizeType length);
	virtual bool ReadDataBigEndian(unsigned char* data, SizeType length);
	virtual bool ReadDataBigEndian(wchar_t* data, SizeType length);
	virtual bool ReadDataBigEndian(short* data, SizeType length);
	virtual bool ReadDataBigEndian(unsigned short* data, SizeType length);
	virtual bool ReadDataBigEndian(int* data, SizeType length);
	virtual bool ReadDataBigEndian(unsigned int* data, SizeType length);
	virtual bool ReadDataBigEndian(long* data, SizeType length);
	virtual bool ReadDataBigEndian(unsigned long* data, SizeType length);
	virtual bool ReadDataBigEndian(long long* data, SizeType length);
	virtual bool ReadDataBigEndian(unsigned long long* data, SizeType length);
	virtual bool ReadDataBigEndian(float* data, SizeType length);
	virtual bool ReadDataBigEndian(double* data, SizeType length);
	virtual bool ReadDataBigEndian(long double* data, SizeType length);
	virtual bool ReadDataLittleEndian(bool* data, SizeType length);
	virtual bool ReadDataLittleEndian(char* data, SizeType length);
	virtual bool ReadDataLittleEndian(signed char* data, SizeType length);
	virtual bool ReadDataLittleEndian(unsigned char* data, SizeType length);
	virtual bool ReadDataLittleEndian(wchar_t* data, SizeType length);
	virtual bool ReadDataLittleEndian(short* data, SizeType length);
	virtual bool ReadDataLittleEndian(unsigned short* data, SizeType length);
	virtual bool ReadDataLittleEndian(int* data, SizeType length);
	virtual bool ReadDataLittleEndian(unsigned int* data, SizeType length);
	virtual bool ReadDataLittleEndian(long* data, SizeType length);
	virtual bool ReadDataLittleEndian(unsigned long* data, SizeType length);
	virtual bool ReadDataLittleEndian(long long* data, SizeType length);
	virtual bool ReadDataLittleEndian(unsigned long long* data, SizeType length);
	virtual bool ReadDataLittleEndian(float* data, SizeType length);
	virtual bool ReadDataLittleEndian(double* data, SizeType length);
	virtual bool ReadDataLittleEndian(long double* data, SizeType length);

	//Text char write functions
	virtual bool WriteChar(const UnicodeCodePoint& data);
	virtual bool WriteCharAsASCII(const UnicodeCodePoint& data);
	virtual bool WriteCharAsUTF8(const UnicodeCodePoint& data);
	virtual bool WriteCharAsUTF16(const UnicodeCodePoint& data);
	virtual bool WriteCharAsUTF32(const UnicodeCodePoint& data);
	virtual bool WriteCharBigEndian(const UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsASCII(const UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsUTF8(const UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsUTF16(const UnicodeCodePoint& data);
	virtual bool WriteCharBigEndianAsUTF32(const UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndian(const UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsASCII(const UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsUTF8(const UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsUTF16(const UnicodeCodePoint& data);
	virtual bool WriteCharLittleEndianAsUTF32(const UnicodeCodePoint& data);

	//Text string write functions
	using IStream::WriteText;
	virtual bool WriteText(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteText(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsASCII(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsUTF8(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsUTF16(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextAsUTF32(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndian(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndian(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsASCII(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsUTF8(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsUTF16(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextBigEndianAsUTF32(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextBigEndianAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndian(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndian(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsASCII(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsUTF8(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsUTF16(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');
	virtual bool WriteTextLittleEndianAsUTF32(const char* data, SizeType bufferSize, char terminator = '\0');
	virtual bool WriteTextLittleEndianAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator = L'\0');

	//Fixed length buffer text write functions
	using IStream::WriteTextFixedLengthBufferAsASCII;
	using IStream::WriteTextFixedLengthBufferAsUTF8;
	using IStream::WriteTextFixedLengthBufferAsUTF16;
	using IStream::WriteTextFixedLengthBufferAsUTF32;
	using IStream::WriteTextBigEndianFixedLengthBufferAsASCII;
	using IStream::WriteTextBigEndianFixedLengthBufferAsUTF8;
	using IStream::WriteTextBigEndianFixedLengthBufferAsUTF16;
	using IStream::WriteTextBigEndianFixedLengthBufferAsUTF32;
	using IStream::WriteTextLittleEndianFixedLengthBufferAsASCII;
	using IStream::WriteTextLittleEndianFixedLengthBufferAsUTF8;
	using IStream::WriteTextLittleEndianFixedLengthBufferAsUTF16;
	using IStream::WriteTextLittleEndianFixedLengthBufferAsUTF32;
	virtual bool WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar = '\0');
	virtual bool WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar = L'\0');

	//Data write functions
	using IStream::WriteData;
	using IStream::WriteDataBigEndian;
	using IStream::WriteDataLittleEndian;
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
	virtual bool WriteData(const bool* data, SizeType length);
	virtual bool WriteData(const char* data, SizeType length);
	virtual bool WriteData(const signed char* data, SizeType length);
	virtual bool WriteData(const unsigned char* data, SizeType length);
	virtual bool WriteData(const wchar_t* data, SizeType length);
	virtual bool WriteData(const short* data, SizeType length);
	virtual bool WriteData(const unsigned short* data, SizeType length);
	virtual bool WriteData(const int* data, SizeType length);
	virtual bool WriteData(const unsigned int* data, SizeType length);
	virtual bool WriteData(const long* data, SizeType length);
	virtual bool WriteData(const unsigned long* data, SizeType length);
	virtual bool WriteData(const long long* data, SizeType length);
	virtual bool WriteData(const unsigned long long* data, SizeType length);
	virtual bool WriteData(const float* data, SizeType length);
	virtual bool WriteData(const double* data, SizeType length);
	virtual bool WriteData(const long double* data, SizeType length);
	virtual bool WriteDataBigEndian(const bool* data, SizeType length);
	virtual bool WriteDataBigEndian(const char* data, SizeType length);
	virtual bool WriteDataBigEndian(const signed char* data, SizeType length);
	virtual bool WriteDataBigEndian(const unsigned char* data, SizeType length);
	virtual bool WriteDataBigEndian(const wchar_t* data, SizeType length);
	virtual bool WriteDataBigEndian(const short* data, SizeType length);
	virtual bool WriteDataBigEndian(const unsigned short* data, SizeType length);
	virtual bool WriteDataBigEndian(const int* data, SizeType length);
	virtual bool WriteDataBigEndian(const unsigned int* data, SizeType length);
	virtual bool WriteDataBigEndian(const long* data, SizeType length);
	virtual bool WriteDataBigEndian(const unsigned long* data, SizeType length);
	virtual bool WriteDataBigEndian(const long long* data, SizeType length);
	virtual bool WriteDataBigEndian(const unsigned long long* data, SizeType length);
	virtual bool WriteDataBigEndian(const float* data, SizeType length);
	virtual bool WriteDataBigEndian(const double* data, SizeType length);
	virtual bool WriteDataBigEndian(const long double* data, SizeType length);
	virtual bool WriteDataLittleEndian(const bool* data, SizeType length);
	virtual bool WriteDataLittleEndian(const char* data, SizeType length);
	virtual bool WriteDataLittleEndian(const signed char* data, SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned char* data, SizeType length);
	virtual bool WriteDataLittleEndian(const wchar_t* data, SizeType length);
	virtual bool WriteDataLittleEndian(const short* data, SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned short* data, SizeType length);
	virtual bool WriteDataLittleEndian(const int* data, SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned int* data, SizeType length);
	virtual bool WriteDataLittleEndian(const long* data, SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned long* data, SizeType length);
	virtual bool WriteDataLittleEndian(const long long* data, SizeType length);
	virtual bool WriteDataLittleEndian(const unsigned long long* data, SizeType length);
	virtual bool WriteDataLittleEndian(const float* data, SizeType length);
	virtual bool WriteDataLittleEndian(const double* data, SizeType length);
	virtual bool WriteDataLittleEndian(const long double* data, SizeType length);

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
	virtual bool ReadBinaryNativeByteOrder(char* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(signed char* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned char* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(wchar_t* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(short* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned short* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(int* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned int* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(long* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned long* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(long long* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(unsigned long long* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(float* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(double* data, SizeType length) = 0;
	virtual bool ReadBinaryNativeByteOrder(long double* data, SizeType length) = 0;

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
	virtual bool WriteBinaryNativeByteOrder(const char* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const signed char* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned char* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const wchar_t* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const short* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned short* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const int* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned int* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const long* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned long* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const long long* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const unsigned long long* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const float* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const double* data, SizeType length) = 0;
	virtual bool WriteBinaryNativeByteOrder(const long double* data, SizeType length) = 0;

private:
	//Internal text format independent char read functions
	inline bool ReadCharInternal(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsASCII(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsUTF8(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsUTF16(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);
	inline bool ReadCharInternalAsUTF32(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn);

	//Internal fixed length text buffer read functions
	inline bool ReadTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar);
	inline bool ReadTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar);

	//Internal type-independent data read functions
	template<class T> bool ReadDataInternal(T& data, ByteOrder abyteOrder);
	template<class T> bool ReadDataInternal(T& data);
	template<class T> bool ReadDataInternalBigEndian(T& data);
	template<class T> bool ReadDataInternalLittleEndian(T& data);
	template<class T> bool ReadDataInternal(T* data, SizeType length, ByteOrder abyteOrder);
	template<class T> bool ReadDataInternal(T* data, SizeType length);
	template<class T> bool ReadDataInternalBigEndian(T* data, SizeType length);
	template<class T> bool ReadDataInternalLittleEndian(T* data, SizeType length);

	//Inverted byte order read functions
	template<class T> bool ReadBinaryInvertedByteOrder(T& data);
	template<class T> bool ReadBinaryInvertedByteOrder(T* data, SizeType length);

	//Internal text format independent char write functions
	inline bool WriteCharInternal(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsASCII(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsUTF8(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsUTF16(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);
	inline bool WriteCharInternalAsUTF32(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn);

	//Text string write functions
	inline bool WriteTextInternal(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator);
	inline bool WriteTextInternal(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsASCII(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsASCII(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsUTF8(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsUTF8(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsUTF16(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsUTF16(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator);
	inline bool WriteTextInternalAsUTF32(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator);
	inline bool WriteTextInternalAsUTF32(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator);

	//Internal fixed length text buffer write functions
	inline bool WriteTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar);
	inline bool WriteTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar);

	//Internal type-independent data write functions
	template<class T> bool WriteDataInternal(T data, ByteOrder abyteOrder);
	template<class T> bool WriteDataInternal(T data);
	template<class T> bool WriteDataInternalBigEndian(T data);
	template<class T> bool WriteDataInternalLittleEndian(T data);
	template<class T> bool WriteDataInternal(const T* data, SizeType length, ByteOrder abyteOrder);
	template<class T> bool WriteDataInternal(const T* data, SizeType length);
	template<class T> bool WriteDataInternalBigEndian(const T* data, SizeType length);
	template<class T> bool WriteDataInternalLittleEndian(const T* data, SizeType length);

	//Inverted byte order write functions
	template<class T> bool WriteBinaryInvertedByteOrder(T data);
	template<class T> bool WriteBinaryInvertedByteOrder(const T* data, SizeType length);

	//Text conversion functions
	static void ConvertASCIIToUnicodeCodePoint(unsigned char source, UnicodeCodePoint& target);
	static void ConvertUTF32ToUnicodeCodePoint(unsigned int source, UnicodeCodePoint& target);
	static void ConvertUTF16ToUnicodeCodePoint(unsigned short codeUnit1, unsigned short codeUnit2, bool surrogatePair, UnicodeCodePoint& target);
	static void ConvertUnicodeCodePointToUTF16(const UnicodeCodePoint& source, unsigned short& codeUnit1, unsigned short& codeUnit2, bool& surrogatePair);
	static void ConvertUnicodeCodePointToUTF32(const UnicodeCodePoint& source, unsigned int& target);
	static bool ConvertCharToUnicodeCodePoint(char source, UnicodeCodePoint& target);
	static bool ConvertWCharTToUnicodeCodePoint(const wchar_t* source, UnicodeCodePoint& target, SizeType bufferSize, SizeType& codeUnitsRead);
	static bool ConvertUnicodeCodePointToChar(const UnicodeCodePoint& source, char& target);
	static bool ConvertUnicodeCodePointToWCharT(const UnicodeCodePoint& source, wchar_t* target, SizeType bufferSize, SizeType& codeUnitsWritten);

	//Bool conversion functions
	inline static unsigned char BoolToByte(bool data);
	inline static bool ByteToBool(unsigned char data);

private:
	ByteOrder byteOrder;
	TextEncoding textEncoding;
	NewLineEncoding newLineEncoding;
};

} //Close namespace Stream
#include "Stream.inl"
#endif
