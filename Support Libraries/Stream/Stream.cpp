#include "Stream.h"
namespace Stream {

//----------------------------------------------------------------------------------------
//Byte order mark functions
//----------------------------------------------------------------------------------------
bool Stream::ProcessByteOrderMark()
{
	//Save the initial stream pos
	SizeType initialStreamPos = GetStreamPos();

	//Read the first two bytes of the file to check for a byte order mark
	unsigned char byte1;
	unsigned char byte2;
	if(!ReadData(byte1) || !ReadData(byte2))
	{
		SetStreamPos(initialStreamPos);
		return false;
	}

	//Check for a UTF-32 big endian mark (00 00 FE FF)
	if((byte1 == 0x00) && (byte2 == 0x00))
	{
		SizeType currentStreamPos = GetStreamPos();
		unsigned char byte3;
		unsigned char byte4;
		if(ReadData(byte3) && ReadData(byte4) && (byte3 == 0xFE) && (byte4 == 0xFF))
		{
			SetTextEncoding(TEXTENCODING_UTF32);
			SetByteOrder(BYTEORDER_BIGENDIAN);
			return true;
		}
		SetStreamPos(currentStreamPos);
	}

	//Check for a UTF-16 big endian mark (FE FF)
	if((byte1 == 0xFE) && (byte2 == 0xFF))
	{
		SetTextEncoding(TEXTENCODING_UTF16);
		SetByteOrder(BYTEORDER_BIGENDIAN);
		return true;
	}

	//Check for a UTF-16 little endian mark (FF FE) or UTF-32 little endian mark (FF FE 00 00)
	if((byte1 == 0xFF) && (byte2 == 0xFE))
	{
		//Check if this actually actually appears to be a UTF-32 mark
		SizeType currentStreamPos = GetStreamPos();
		unsigned char byte3;
		unsigned char byte4;
		if(ReadData(byte3) && ReadData(byte4) && (byte3 == 0x00) && (byte4 == 0x00))
		{
			SetTextEncoding(TEXTENCODING_UTF32);
			SetByteOrder(BYTEORDER_LITTLEENDIAN);
			return true;
		}
		else
		{
			SetStreamPos(currentStreamPos);
			SetTextEncoding(TEXTENCODING_UTF16);
			SetByteOrder(BYTEORDER_LITTLEENDIAN);
			return true;
		}
	}

	//Check for UTF-8 mark (EF BB BF)
	if((byte1 == 0xEF) && (byte2 == 0xBB))
	{
		SizeType currentStreamPos = GetStreamPos();
		unsigned char byte3;
		if(ReadData(byte3) && (byte3 == 0xBF))
		{
			SetTextEncoding(TEXTENCODING_UTF8);
			return true;
		}
		SetStreamPos(currentStreamPos);
	}

	SetStreamPos(initialStreamPos);
	return false;
}

//----------------------------------------------------------------------------------------
void Stream::InsertByteOrderMark()
{
	switch(textEncoding)
	{
	case TEXTENCODING_UTF8:{
		unsigned char byte1 = 0xEF;
		unsigned char byte2 = 0xBB;
		unsigned char byte3 = 0xBF;
		WriteData(byte1);
		WriteData(byte2);
		WriteData(byte3);
		break;}
	case TEXTENCODING_UTF16:{
		unsigned short data = 0xFEFF;
		WriteData(data);
		break;}
	case TEXTENCODING_UTF32:{
		unsigned short data = 0x0000FEFF;
		WriteData(data);
		break;}
	}
}

//----------------------------------------------------------------------------------------
//Text encoding functions
//----------------------------------------------------------------------------------------
Stream::TextEncoding Stream::GetTextEncoding() const
{
	return textEncoding;
}

//----------------------------------------------------------------------------------------
void Stream::SetTextEncoding(TextEncoding atextEncoding)
{
	textEncoding = atextEncoding;
}

//----------------------------------------------------------------------------------------
//New line encoding functions
//----------------------------------------------------------------------------------------
Stream::NewLineEncoding Stream::GetNewLineEncoding() const
{
	return newLineEncoding;
}

//----------------------------------------------------------------------------------------
void Stream::SetNewLineEncoding(NewLineEncoding anewLineEncoding)
{
	newLineEncoding = anewLineEncoding;
}

//----------------------------------------------------------------------------------------
//Byte order functions
//----------------------------------------------------------------------------------------
Stream::ByteOrder Stream::GetByteOrder() const
{
	return byteOrder;
}

//----------------------------------------------------------------------------------------
void Stream::SetByteOrder(ByteOrder abyteOrder)
{
	byteOrder = abyteOrder;
}

//----------------------------------------------------------------------------------------
//Text char read functions
//----------------------------------------------------------------------------------------
bool Stream::ReadChar(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternal(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharAsASCII(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsASCII(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharAsUTF8(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF8(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharAsUTF16(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF16(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharAsUTF32(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF32(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharBigEndian(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternal(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharBigEndianAsASCII(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsASCII(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharBigEndianAsUTF8(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF8(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharBigEndianAsUTF16(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF16(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharBigEndianAsUTF32(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF32(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharLittleEndian(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternal(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharLittleEndianAsASCII(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsASCII(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharLittleEndianAsUTF8(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF8(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharLittleEndianAsUTF16(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF16(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharLittleEndianAsUTF32(UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF32(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
//Fixed length text buffer read functions
//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
//Data read functions
//----------------------------------------------------------------------------------------
bool Stream::ReadData(bool& data)
{
	bool result;
	unsigned char temp;
	result = ReadDataInternal(temp);
	data = ByteToBool(temp);
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(char& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(signed char& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned char& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(wchar_t& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(short& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned short& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(int& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned int& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(long long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned long long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(float& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(double& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(long double& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(bool& data)
{
	bool result;
	unsigned char temp;
	result = ReadDataInternalBigEndian(temp);
	data = ByteToBool(temp);
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(char& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(signed char& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned char& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(wchar_t& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(short& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned short& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(int& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned int& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(long long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned long long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(float& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(double& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(long double& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(bool& data)
{
	bool result;
	unsigned char temp;
	result = ReadDataInternalLittleEndian(temp);
	data = ByteToBool(temp);
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(char& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(signed char& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned char& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(wchar_t& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(short& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned short& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(int& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned int& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(long long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned long long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(float& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(double& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(long double& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
//Array read functions
//----------------------------------------------------------------------------------------
bool Stream::ReadData(bool* data, SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		unsigned char temp;
		result &= ReadDataInternal(temp);
		data[length] = ByteToBool(temp);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(char* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(signed char* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned char* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(wchar_t* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(short* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned short* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(int* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned int* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(long* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned long* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(long long* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(unsigned long long* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(float* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(double* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadData(long double* data, SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(bool* data, SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		unsigned char temp;
		result &= ReadDataInternalBigEndian(temp);
		data[length] = ByteToBool(temp);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(char* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(signed char* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned char* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(wchar_t* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(short* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned short* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(int* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned int* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(long* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned long* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(long long* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(unsigned long long* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(float* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(double* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataBigEndian(long double* data, SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(bool* data, SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		unsigned char temp;
		result &= ReadDataInternalLittleEndian(temp);
		data[length] = ByteToBool(temp);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(char* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(signed char* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned char* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(wchar_t* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(short* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned short* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(int* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned int* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(long* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned long* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(long long* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(unsigned long long* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(float* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(double* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::ReadDataLittleEndian(long double* data, SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
//Text char write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteChar(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternal(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharAsASCII(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsASCII(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharAsUTF8(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF8(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharAsUTF16(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF16(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharAsUTF32(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF32(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharBigEndian(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternal(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharBigEndianAsASCII(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsASCII(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharBigEndianAsUTF8(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF8(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharBigEndianAsUTF16(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF16(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharBigEndianAsUTF32(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF32(data, BYTEORDER_BIGENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharLittleEndian(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternal(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharLittleEndianAsASCII(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsASCII(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharLittleEndianAsUTF8(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF8(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharLittleEndianAsUTF16(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF16(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharLittleEndianAsUTF32(const UnicodeCodePoint& data)
{
	SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF32(data, BYTEORDER_LITTLEENDIAN, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
//Text string write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteText(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternal(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteText(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternal(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsASCII(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsASCII(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsASCII(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsUTF8(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF8(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF8(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsUTF16(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF16(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF16(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsUTF32(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF32(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF32(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndian(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternal(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndian(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternal(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsASCII(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsASCII(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsASCII(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsUTF8(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF8(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF8(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsUTF16(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF16(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF16(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsUTF32(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF32(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF32(BYTEORDER_BIGENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndian(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternal(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndian(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternal(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsASCII(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsASCII(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsASCII(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsASCII(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsUTF8(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF8(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsUTF8(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF8(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsUTF16(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF16(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsUTF16(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF16(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsUTF32(const char* data, SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF32(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianAsUTF32(const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF32(BYTEORDER_LITTLEENDIAN, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
//Fixed length buffer text write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_BIGENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(BYTEORDER_LITTLEENDIAN, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
//Data write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteData(bool data)
{
	return WriteDataInternal(BoolToByte(data));
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(char data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(signed char data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(unsigned char data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(wchar_t data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(short data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(unsigned short data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(int data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(unsigned int data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(unsigned long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(long long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(unsigned long long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(float data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(double data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(long double data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(bool data)
{
	return WriteDataInternalBigEndian(BoolToByte(data));
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(char data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(signed char data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(unsigned char data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(wchar_t data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(short data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(unsigned short data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(int data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(unsigned int data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(unsigned long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(long long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(unsigned long long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(float data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(double data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(long double data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(bool data)
{
	return WriteDataInternalLittleEndian(BoolToByte(data));
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(char data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(signed char data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(unsigned char data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(wchar_t data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(short data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(unsigned short data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(int data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(unsigned int data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(unsigned long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(long long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(unsigned long long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(float data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(double data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(long double data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
//Array write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteData(const bool* data, SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		result &= WriteDataInternal(BoolToByte(data[i]));
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const signed char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const unsigned char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const wchar_t* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const short* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const unsigned short* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const int* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const unsigned int* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const unsigned long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const long long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const unsigned long long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const float* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const double* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteData(const long double* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const bool* data, SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		result &= WriteDataInternalBigEndian(BoolToByte(data[i]));
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const signed char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const unsigned char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const wchar_t* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const short* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const unsigned short* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const int* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const unsigned int* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const unsigned long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const long long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const unsigned long long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const float* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const double* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataBigEndian(const long double* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const bool* data, SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		result &= WriteDataInternalLittleEndian(BoolToByte(data[i]));
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const signed char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const unsigned char* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const wchar_t* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const short* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const unsigned short* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const int* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const unsigned int* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const unsigned long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const long long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const unsigned long long* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const float* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const double* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
bool Stream::WriteDataLittleEndian(const long double* data, SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
//Text conversion functions
//----------------------------------------------------------------------------------------
void Stream::ConvertASCIIToUnicodeCodePoint(unsigned char source, UnicodeCodePoint& target)
{
	target.surrogatePair = false;
	target.codeUnit1 = (wchar_t)source;
}

//----------------------------------------------------------------------------------------
void Stream::ConvertUTF16ToUnicodeCodePoint(unsigned short codeUnit1, unsigned short codeUnit2, bool surrogatePair, UnicodeCodePoint& target)
{
	//##TODO## Handle platforms where wchar_t uses UTF32 encoding
	target.surrogatePair = surrogatePair;
	target.codeUnit1 = (wchar_t)codeUnit1;
	target.codeUnit2 = (wchar_t)codeUnit2;
}

//----------------------------------------------------------------------------------------
void Stream::ConvertUTF32ToUnicodeCodePoint(unsigned int source, UnicodeCodePoint& target)
{
	//##TODO## Handle platforms where wchar_t uses UTF32 encoding
	if((source & 0xFFFF0000) == 0)
	{
		target.surrogatePair = false;
		target.codeUnit1 = (wchar_t)source;
	}
	else
	{
		target.surrogatePair = true;
		target.codeUnit1 = (wchar_t)((((source - 0x10000) >> 10) & 0x03FF) | 0xD800);
		target.codeUnit2 = (wchar_t)(((source - 0x10000) & 0x03FF) | 0xDC00);
	}
}

//----------------------------------------------------------------------------------------
void Stream::ConvertUnicodeCodePointToUTF16(const UnicodeCodePoint& source, unsigned short& codeUnit1, unsigned short& codeUnit2, bool& surrogatePair)
{
	//##TODO## Handle platforms where wchar_t uses UTF32 encoding
	surrogatePair = source.surrogatePair;
	codeUnit1 = source.codeUnit1;
	codeUnit2 = source.codeUnit2;
}

//----------------------------------------------------------------------------------------
void Stream::ConvertUnicodeCodePointToUTF32(const UnicodeCodePoint& source, unsigned int& target)
{
	//##TODO## Handle platforms where wchar_t uses UTF32 encoding
	if(!source.surrogatePair)
	{
		target = (unsigned int)((unsigned short)source.codeUnit1);
	}
	else
	{
		target = ((unsigned int)(source.codeUnit1 & 0x03FF) << 10) | (unsigned int)(source.codeUnit2 & 0x3FF);
	}
}

//----------------------------------------------------------------------------------------
bool Stream::ConvertCharToUnicodeCodePoint(char source, UnicodeCodePoint& target)
{
	target.codeUnit1 = (wchar_t)((unsigned char)source);
	target.surrogatePair = false;
	return true;
}

//----------------------------------------------------------------------------------------
bool Stream::ConvertWCharTToUnicodeCodePoint(const wchar_t* source, UnicodeCodePoint& target, SizeType bufferSize, SizeType& codeUnitsRead)
{
	if((bufferSize - codeUnitsRead) <= 0)
	{
		return false;
	}
	unsigned short codeUnit1;
	codeUnit1 = (unsigned short)*source;
	++codeUnitsRead;

	//If this is the second half of a surrogate pair, the data is invalid. Abort the
	//read.
	if((codeUnit1 & 0xFC00) == 0xDC00)
	{
		return false;
	}

	//If this is the first half of a surrogate pair, read in the second half.
	bool surrogatePair = false;
	unsigned short codeUnit2 = 0;
	if((codeUnit1 & 0xFC00) == 0xD800)
	{
		//Ensure that code units are available in the buffer
		if((bufferSize - codeUnitsRead) <= 0)
		{
			return false;
		}

		//Read in the second code unit
		codeUnit2 = (unsigned short)*(source + 1);
		++codeUnitsRead;
		surrogatePair = true;

		//If this is isn't the second half of a surrogate pair, the data is invalid.
		//Abort the read.
		if((codeUnit2 & 0xFC00) != 0xDC00)
		{
			return false;
		}
	}
	ConvertUTF16ToUnicodeCodePoint(codeUnit1, codeUnit2, surrogatePair, target);
	return true;
}

//----------------------------------------------------------------------------------------
bool Stream::ConvertUnicodeCodePointToChar(const UnicodeCodePoint& source, char& target)
{
	//##TODO## Convert unsupported characters to something else?
	target = (char)source.codeUnit1;
	return true;
}

//----------------------------------------------------------------------------------------
bool Stream::ConvertUnicodeCodePointToWCharT(const UnicodeCodePoint& source, wchar_t* target, SizeType bufferSize, SizeType& codeUnitsWritten)
{
	if((bufferSize - codeUnitsWritten) <= 0)
	{
		return false;
	}
	*target = source.codeUnit1;
	++codeUnitsWritten;
	if(source.surrogatePair)
	{
		if((bufferSize - codeUnitsWritten) <= 0)
		{
			return false;
		}
		*(target + 1) = source.codeUnit2;
		++codeUnitsWritten;
	}
	return true;
}

} //Close namespace Stream
