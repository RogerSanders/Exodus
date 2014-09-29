#include "Stream.h"
namespace Stream {

//----------------------------------------------------------------------------------------
//Explicit template instantiation
//----------------------------------------------------------------------------------------
template class Stream<IStream>;
template class Stream<IStreamNonSeekable>;

//----------------------------------------------------------------------------------------
//Byte order mark functions
//----------------------------------------------------------------------------------------
template<> bool Stream<IStream>::ProcessByteOrderMark()
{
	//Save the initial stream pos
	IStream::SizeType initialStreamPos = GetStreamPos();

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
		IStream::SizeType currentStreamPos = GetStreamPos();
		unsigned char byte3;
		unsigned char byte4;
		if(ReadData(byte3) && ReadData(byte4) && (byte3 == 0xFE) && (byte4 == 0xFF))
		{
			SetTextEncoding(IStream::TextEncoding::UTF32);
			SetByteOrder(IStream::ByteOrder::BigEndian);
			return true;
		}
		SetStreamPos(currentStreamPos);
	}

	//Check for a UTF-16 big endian mark (FE FF)
	if((byte1 == 0xFE) && (byte2 == 0xFF))
	{
		SetTextEncoding(IStream::TextEncoding::UTF16);
		SetByteOrder(IStream::ByteOrder::BigEndian);
		return true;
	}

	//Check for a UTF-16 little endian mark (FF FE) or UTF-32 little endian mark (FF FE 00 00)
	if((byte1 == 0xFF) && (byte2 == 0xFE))
	{
		//Check if this actually actually appears to be a UTF-32 mark
		IStream::SizeType currentStreamPos = GetStreamPos();
		unsigned char byte3;
		unsigned char byte4;
		if(ReadData(byte3) && ReadData(byte4) && (byte3 == 0x00) && (byte4 == 0x00))
		{
			SetTextEncoding(IStream::TextEncoding::UTF32);
			SetByteOrder(IStream::ByteOrder::LittleEndian);
			return true;
		}
		else
		{
			SetStreamPos(currentStreamPos);
			SetTextEncoding(IStream::TextEncoding::UTF16);
			SetByteOrder(IStream::ByteOrder::LittleEndian);
			return true;
		}
	}

	//Check for UTF-8 mark (EF BB BF)
	if((byte1 == 0xEF) && (byte2 == 0xBB))
	{
		IStream::SizeType currentStreamPos = GetStreamPos();
		unsigned char byte3;
		if(ReadData(byte3) && (byte3 == 0xBF))
		{
			SetTextEncoding(IStream::TextEncoding::UTF8);
			return true;
		}
		SetStreamPos(currentStreamPos);
	}

	SetStreamPos(initialStreamPos);
	return false;
}

//----------------------------------------------------------------------------------------
template<> bool Stream<IStreamNonSeekable>::ProcessByteOrderMark()
{
	//Read the first two bytes of the file to check for a byte order mark
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
	unsigned char byte4;
	if(!ReadData(byte1) || !ReadData(byte2) || !ReadData(byte3) || !ReadData(byte4))
	{
		return false;
	}

	//Check for a UTF-32 big endian mark (00 00 FE FF)
	if((byte1 == 0x00) && (byte2 == 0x00) && (byte3 == 0xFE) && (byte4 == 0xFF))
	{
		SetTextEncoding(IStream::TextEncoding::UTF32);
		SetByteOrder(IStream::ByteOrder::BigEndian);
		return true;
	}

	//Check for a UTF-16 big endian mark (FE FF)
	if((byte1 == 0xFE) && (byte2 == 0xFF))
	{
		SetTextEncoding(IStream::TextEncoding::UTF16);
		SetByteOrder(IStream::ByteOrder::BigEndian);
		return true;
	}

	//Check for a UTF-16 little endian mark (FF FE) or UTF-32 little endian mark (FF FE 00 00)
	if((byte1 == 0xFF) && (byte2 == 0xFE))
	{
		//Check if this actually actually appears to be a UTF-32 mark
		if((byte3 == 0x00) && (byte4 == 0x00))
		{
			SetTextEncoding(IStream::TextEncoding::UTF32);
			SetByteOrder(IStream::ByteOrder::LittleEndian);
			return true;
		}
		else
		{
			SetTextEncoding(IStream::TextEncoding::UTF16);
			SetByteOrder(IStream::ByteOrder::LittleEndian);
			return true;
		}
	}

	//Check for UTF-8 mark (EF BB BF)
	if((byte1 == 0xEF) && (byte2 == 0xBB) && (byte3 == 0xBF))
	{
		SetTextEncoding(IStream::TextEncoding::UTF8);
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::InsertByteOrderMark()
{
	switch(textEncoding)
	{
	case B::TextEncoding::UTF8:{
		unsigned char byte1 = 0xEF;
		unsigned char byte2 = 0xBB;
		unsigned char byte3 = 0xBF;
		WriteData(byte1);
		WriteData(byte2);
		WriteData(byte3);
		break;}
	case B::TextEncoding::UTF16:{
		unsigned short data = 0xFEFF;
		WriteData(data);
		break;}
	case B::TextEncoding::UTF32:{
		unsigned short data = 0x0000FEFF;
		WriteData(data);
		break;}
	}
}

//----------------------------------------------------------------------------------------
//Text encoding functions
//----------------------------------------------------------------------------------------
template<class B> typename B::TextEncoding Stream<B>::GetTextEncoding() const
{
	return textEncoding;
}

//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::SetTextEncoding(typename B::TextEncoding atextEncoding)
{
	textEncoding = atextEncoding;
}

//----------------------------------------------------------------------------------------
//New line encoding functions
//----------------------------------------------------------------------------------------
template<class B> typename B::NewLineEncoding Stream<B>::GetNewLineEncoding() const
{
	return newLineEncoding;
}

//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::SetNewLineEncoding(typename B::NewLineEncoding anewLineEncoding)
{
	newLineEncoding = anewLineEncoding;
}

//----------------------------------------------------------------------------------------
//Byte order functions
//----------------------------------------------------------------------------------------
template<class B> typename B::ByteOrder Stream<B>::GetByteOrder() const
{
	return byteOrder;
}

//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::SetByteOrder(typename B::ByteOrder abyteOrder)
{
	byteOrder = abyteOrder;
}

//----------------------------------------------------------------------------------------
//Text char read functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadChar(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternal(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharAsASCII(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsASCII(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharAsUTF8(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF8(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharAsUTF16(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF16(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharAsUTF32(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF32(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharBigEndian(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternal(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharBigEndianAsASCII(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsASCII(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharBigEndianAsUTF8(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF8(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharBigEndianAsUTF16(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF16(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharBigEndianAsUTF32(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF32(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharLittleEndian(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternal(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharLittleEndianAsASCII(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsASCII(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharLittleEndianAsUTF8(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF8(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharLittleEndianAsUTF16(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF16(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadCharLittleEndianAsUTF32(typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return ReadCharInternalAsUTF32(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
//Fixed length text buffer read functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, char* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, char paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, typename B::SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	return ReadTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
}

//----------------------------------------------------------------------------------------
//Data read functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(bool& data)
{
	bool result;
	unsigned char temp;
	result = ReadDataInternal(temp);
	data = ByteToBool(temp);
	return result;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(char& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(signed char& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned char& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(wchar_t& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(short& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned short& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(int& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned int& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(long long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned long long& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(float& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(double& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(long double& data)
{
	return ReadDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(bool& data)
{
	bool result;
	unsigned char temp;
	result = ReadDataInternalBigEndian(temp);
	data = ByteToBool(temp);
	return result;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(char& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(signed char& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned char& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(wchar_t& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(short& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned short& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(int& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned int& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(long long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned long long& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(float& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(double& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(long double& data)
{
	return ReadDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(bool& data)
{
	bool result;
	unsigned char temp;
	result = ReadDataInternalLittleEndian(temp);
	data = ByteToBool(temp);
	return result;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(char& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(signed char& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned char& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(wchar_t& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(short& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned short& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(int& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned int& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(long long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned long long& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(float& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(double& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(long double& data)
{
	return ReadDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
//Array read functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(bool* data, typename B::SizeType length)
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
template<class B> bool Stream<B>::ReadData(char* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(signed char* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned char* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(wchar_t* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(short* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned short* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(int* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned int* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(long* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned long* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(long long* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(unsigned long long* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(float* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(double* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadData(long double* data, typename B::SizeType length)
{
	return ReadDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(bool* data, typename B::SizeType length)
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
template<class B> bool Stream<B>::ReadDataBigEndian(char* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(signed char* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned char* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(wchar_t* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(short* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned short* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(int* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned int* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(long* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned long* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(long long* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(unsigned long long* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(float* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(double* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataBigEndian(long double* data, typename B::SizeType length)
{
	return ReadDataInternalBigEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(bool* data, typename B::SizeType length)
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
template<class B> bool Stream<B>::ReadDataLittleEndian(char* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(signed char* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned char* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(wchar_t* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(short* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned short* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(int* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned int* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(long* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned long* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(long long* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(unsigned long long* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(float* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(double* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ReadDataLittleEndian(long double* data, typename B::SizeType length)
{
	return ReadDataInternalLittleEndian(data, length);
}

//----------------------------------------------------------------------------------------
//Text char write functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteChar(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternal(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharAsASCII(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsASCII(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharAsUTF8(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF8(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharAsUTF16(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF16(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharAsUTF32(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF32(data, byteOrder, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharBigEndian(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternal(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharBigEndianAsASCII(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsASCII(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharBigEndianAsUTF8(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF8(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharBigEndianAsUTF16(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF16(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharBigEndianAsUTF32(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF32(data, B::ByteOrder::BigEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharLittleEndian(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternal(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharLittleEndianAsASCII(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsASCII(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharLittleEndianAsUTF8(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF8(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharLittleEndianAsUTF16(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF16(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteCharLittleEndianAsUTF32(const typename B::UnicodeCodePoint& data)
{
	typename B::SizeType remainingCodeUnitsAvailable = 9999;
	return WriteCharInternalAsUTF32(data, B::ByteOrder::LittleEndian, remainingCodeUnitsAvailable, true);
}

//----------------------------------------------------------------------------------------
//Text string write functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteText(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternal(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteText(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternal(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsASCII(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsASCII(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsASCII(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsASCII(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsUTF8(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF8(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsUTF8(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF8(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsUTF16(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF16(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsUTF16(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF16(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsUTF32(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF32(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextAsUTF32(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF32(byteOrder, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndian(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternal(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndian(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternal(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsASCII(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsASCII(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsASCII(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsASCII(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsUTF8(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF8(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsUTF8(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF8(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsUTF16(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF16(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsUTF16(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF16(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsUTF32(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF32(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianAsUTF32(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF32(B::ByteOrder::BigEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndian(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternal(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndian(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternal(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsASCII(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsASCII(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsASCII(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsASCII(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsUTF8(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF8(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsUTF8(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF8(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsUTF16(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF16(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsUTF16(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF16(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsUTF32(const char* data, typename B::SizeType bufferSize, char terminator)
{
	return WriteTextInternalAsUTF32(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianAsUTF32(const wchar_t* data, typename B::SizeType bufferSize, wchar_t terminator)
{
	return WriteTextInternalAsUTF32(B::ByteOrder::LittleEndian, data, bufferSize, terminator);
}

//----------------------------------------------------------------------------------------
//Fixed length buffer text write functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(byteOrder, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextBigEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::BigEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsASCII(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsUTF8(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsUTF16(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const char* memoryBuffer, typename B::SizeType codeUnitsInMemory, char paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteTextLittleEndianFixedLengthBufferAsUTF32(typename B::SizeType codeUnitsInStream, const wchar_t* memoryBuffer, typename B::SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	return WriteTextInternalFixedLengthBufferAsASCII(B::ByteOrder::LittleEndian, codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
}

//----------------------------------------------------------------------------------------
//Data write functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(bool data)
{
	return WriteDataInternal(BoolToByte(data));
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(char data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(signed char data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(unsigned char data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(wchar_t data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(short data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(unsigned short data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(int data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(unsigned int data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(unsigned long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(long long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(unsigned long long data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(float data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(double data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(long double data)
{
	return WriteDataInternal(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(bool data)
{
	return WriteDataInternalBigEndian(BoolToByte(data));
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(char data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(signed char data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(unsigned char data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(wchar_t data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(short data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(unsigned short data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(int data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(unsigned int data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(unsigned long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(long long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(unsigned long long data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(float data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(double data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(long double data)
{
	return WriteDataInternalBigEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(bool data)
{
	return WriteDataInternalLittleEndian(BoolToByte(data));
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(char data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(signed char data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(unsigned char data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(wchar_t data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(short data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(unsigned short data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(int data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(unsigned int data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(unsigned long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(long long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(unsigned long long data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(float data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(double data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(long double data)
{
	return WriteDataInternalLittleEndian(data);
}

//----------------------------------------------------------------------------------------
//Array write functions
//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const bool* data, typename B::SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		result &= WriteDataInternal(BoolToByte(data[i]));
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const signed char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const unsigned char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const wchar_t* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const short* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const unsigned short* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const int* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const unsigned int* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const unsigned long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const long long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const unsigned long long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const float* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const double* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteData(const long double* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const bool* data, typename B::SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		result &= WriteDataInternalBigEndian(BoolToByte(data[i]));
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const signed char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const unsigned char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const wchar_t* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const short* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const unsigned short* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const int* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const unsigned int* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const unsigned long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const long long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const unsigned long long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const float* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const double* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataBigEndian(const long double* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const bool* data, typename B::SizeType length)
{
	bool result = true;
	for(unsigned int i = 0; i < length; ++i)
	{
		result &= WriteDataInternalLittleEndian(BoolToByte(data[i]));
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const signed char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const unsigned char* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const wchar_t* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const short* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const unsigned short* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const int* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const unsigned int* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const unsigned long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const long long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const unsigned long long* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const float* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const double* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::WriteDataLittleEndian(const long double* data, typename B::SizeType length)
{
	return WriteDataInternal(data, length);
}

//----------------------------------------------------------------------------------------
//Text conversion functions
//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::ConvertASCIIToUnicodeCodePoint(unsigned char source, typename B::UnicodeCodePoint& target)
{
	target.surrogatePair = false;
	target.codeUnit1 = (wchar_t)source;
}

//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::ConvertUTF16ToUnicodeCodePoint(unsigned short codeUnit1, unsigned short codeUnit2, bool surrogatePair, typename B::UnicodeCodePoint& target)
{
	//##TODO## Handle platforms where wchar_t uses UTF32 encoding
	target.surrogatePair = surrogatePair;
	target.codeUnit1 = (wchar_t)codeUnit1;
	target.codeUnit2 = (wchar_t)codeUnit2;
}

//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::ConvertUTF32ToUnicodeCodePoint(unsigned int source, typename B::UnicodeCodePoint& target)
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
template<class B> void Stream<B>::ConvertUnicodeCodePointToUTF16(const typename B::UnicodeCodePoint& source, unsigned short& codeUnit1, unsigned short& codeUnit2, bool& surrogatePair)
{
	//##TODO## Handle platforms where wchar_t uses UTF32 encoding
	surrogatePair = source.surrogatePair;
	codeUnit1 = source.codeUnit1;
	codeUnit2 = source.codeUnit2;
}

//----------------------------------------------------------------------------------------
template<class B> void Stream<B>::ConvertUnicodeCodePointToUTF32(const typename B::UnicodeCodePoint& source, unsigned int& target)
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
template<class B> bool Stream<B>::ConvertCharToUnicodeCodePoint(char source, typename B::UnicodeCodePoint& target)
{
	target.codeUnit1 = (wchar_t)((unsigned char)source);
	target.surrogatePair = false;
	return true;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ConvertWCharTToUnicodeCodePoint(const wchar_t* source, typename B::UnicodeCodePoint& target, typename B::SizeType bufferSize, typename B::SizeType& codeUnitsRead)
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
template<class B> bool Stream<B>::ConvertUnicodeCodePointToChar(const typename B::UnicodeCodePoint& source, char& target)
{
	//##TODO## Convert unsupported characters to something else?
	target = (char)source.codeUnit1;
	return true;
}

//----------------------------------------------------------------------------------------
template<class B> bool Stream<B>::ConvertUnicodeCodePointToWCharT(const typename B::UnicodeCodePoint& source, wchar_t* target, typename B::SizeType bufferSize, typename B::SizeType& codeUnitsWritten)
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
