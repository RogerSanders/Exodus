namespace Stream {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IStreamNonSeekable::ByteOrder
{
	Platform,
	LittleEndian,
	BigEndian
};

//----------------------------------------------------------------------------------------
enum class IStreamNonSeekable::TextEncoding
{
	ASCII,
	UTF8,
	UTF16,
	UTF32
};

//----------------------------------------------------------------------------------------
enum class IStreamNonSeekable::NewLineEncoding
{
	Unix,
	DOS
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IStreamNonSeekable::UnicodeCodePoint
{
	wchar_t codeUnit1;
	wchar_t codeUnit2;
	bool surrogatePair;
};

//----------------------------------------------------------------------------------------
//Text char read functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadChar(ByteOrder byteOrder, UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadChar(data);
	case ByteOrder::LittleEndian:
		return ReadCharLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadCharBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadCharAsASCII(ByteOrder byteOrder, UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadCharAsASCII(data);
	case ByteOrder::LittleEndian:
		return ReadCharLittleEndianAsASCII(data);
	case ByteOrder::BigEndian:
		return ReadCharBigEndianAsASCII(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadCharAsUTF8(ByteOrder byteOrder, UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadCharAsUTF8(data);
	case ByteOrder::LittleEndian:
		return ReadCharLittleEndianAsUTF8(data);
	case ByteOrder::BigEndian:
		return ReadCharBigEndianAsUTF8(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadCharAsUTF16(ByteOrder byteOrder, UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadCharAsUTF16(data);
	case ByteOrder::LittleEndian:
		return ReadCharLittleEndianAsUTF16(data);
	case ByteOrder::BigEndian:
		return ReadCharBigEndianAsUTF16(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadCharAsUTF32(ByteOrder byteOrder, UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadCharAsUTF32(data);
	case ByteOrder::LittleEndian:
		return ReadCharLittleEndianAsUTF32(data);
	case ByteOrder::BigEndian:
		return ReadCharBigEndianAsUTF32(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Fixed length text buffer read functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, codeUnitsWritten, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return ReadTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
//Data read functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, bool& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, char& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, signed char& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned char& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, wchar_t& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, short& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned short& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, int& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned int& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, long& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned long& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, long long& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned long long& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, float& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, double& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, long double& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Array read functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, bool* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, char* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, signed char* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned char* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, wchar_t* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, short* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned short* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, int* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned int* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, long long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, unsigned long long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, float* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, double* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, long double* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::ReadData(ByteOrder byteOrder, std::vector<T>& data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return ReadData(data, length);
	case ByteOrder::LittleEndian:
		return ReadDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return ReadDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::ReadData(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if (length <= 0)
	{
		return true;
	}
	return ReadData(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::ReadDataBigEndian(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if (length <= 0)
	{
		return true;
	}
	return ReadDataBigEndian(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::ReadDataLittleEndian(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if (length <= 0)
	{
		return true;
	}
	return ReadDataLittleEndian(&data[0], length);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadData(std::vector<bool>& data, SizeType length)
{
	bool result = true;
	size_t elementCount = (size_t)length;
	data.resize(elementCount);
	for (size_t i = 0; i < elementCount; ++i)
	{
		bool temp;
		result &= ReadData(temp);
		data[i] = temp;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadDataBigEndian(std::vector<bool>& data, SizeType length)
{
	bool result = true;
	size_t elementCount = (size_t)length;
	data.resize(elementCount);
	for (size_t i = 0; i < elementCount; ++i)
	{
		bool temp;
		result &= ReadDataBigEndian(temp);
		data[i] = temp;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::ReadDataLittleEndian(std::vector<bool>& data, SizeType length)
{
	bool result = true;
	size_t elementCount = (size_t)length;
	data.resize(elementCount);
	for (size_t i = 0; i < elementCount; ++i)
	{
		bool temp;
		result &= ReadDataLittleEndian(temp);
		data[i] = temp;
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Text char write functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteChar(ByteOrder byteOrder, const UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteChar(data);
	case ByteOrder::LittleEndian:
		return WriteCharLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteCharBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteCharAsASCII(ByteOrder byteOrder, const UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteCharAsASCII(data);
	case ByteOrder::LittleEndian:
		return WriteCharLittleEndianAsASCII(data);
	case ByteOrder::BigEndian:
		return WriteCharBigEndianAsASCII(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteCharAsUTF8(ByteOrder byteOrder, const UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteCharAsUTF8(data);
	case ByteOrder::LittleEndian:
		return WriteCharLittleEndianAsUTF8(data);
	case ByteOrder::BigEndian:
		return WriteCharBigEndianAsUTF8(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteCharAsUTF16(ByteOrder byteOrder, const UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteCharAsUTF16(data);
	case ByteOrder::LittleEndian:
		return WriteCharLittleEndianAsUTF16(data);
	case ByteOrder::BigEndian:
		return WriteCharBigEndianAsUTF16(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteCharAsUTF32(ByteOrder byteOrder, const UnicodeCodePoint& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteCharAsUTF32(data);
	case ByteOrder::LittleEndian:
		return WriteCharLittleEndianAsUTF32(data);
	case ByteOrder::BigEndian:
		return WriteCharBigEndianAsUTF32(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Text string write functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteText(ByteOrder byteOrder, const char* data, SizeType bufferSize, char terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteText(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndian(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndian(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteText(ByteOrder byteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteText(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndian(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndian(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsASCII(ByteOrder byteOrder, const char* data, SizeType bufferSize, char terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsASCII(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsASCII(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsASCII(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsASCII(ByteOrder byteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsASCII(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsASCII(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsASCII(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF8(ByteOrder byteOrder, const char* data, SizeType bufferSize, char terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF8(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF8(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF8(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF8(ByteOrder byteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF8(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF8(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF8(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF16(ByteOrder byteOrder, const char* data, SizeType bufferSize, char terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF16(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF16(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF16(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF16(ByteOrder byteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF16(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF16(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF16(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF32(ByteOrder byteOrder, const char* data, SizeType bufferSize, char terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF32(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF32(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF32(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF32(ByteOrder byteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF32(data, bufferSize, terminator);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF32(data, bufferSize, terminator);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF32(data, bufferSize, terminator);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteText(ByteOrder byteOrder, const std::string& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteText(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteText(ByteOrder byteOrder, const std::wstring& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteText(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsASCII(ByteOrder byteOrder, const std::string& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsASCII(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsASCII(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsASCII(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsASCII(ByteOrder byteOrder, const std::wstring& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsASCII(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsASCII(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsASCII(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF8(ByteOrder byteOrder, const std::string& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF8(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF8(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF8(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF8(ByteOrder byteOrder, const std::wstring& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF8(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF8(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF8(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF16(ByteOrder byteOrder, const std::string& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF16(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF16(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF16(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF16(ByteOrder byteOrder, const std::wstring& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF16(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF16(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF16(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF32(ByteOrder byteOrder, const std::string& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF32(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF32(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF32(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF32(ByteOrder byteOrder, const std::wstring& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextAsUTF32(data);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianAsUTF32(data);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianAsUTF32(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteText(const std::string& data)
{
	return WriteText(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteText(const std::wstring& data)
{
	return WriteText(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF8(const std::string& data)
{
	return WriteTextAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF8(const std::wstring& data)
{
	return WriteTextAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF16(const std::string& data)
{
	return WriteTextAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF16(const std::wstring& data)
{
	return WriteTextAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF32(const std::string& data)
{
	return WriteTextAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextAsUTF32(const std::wstring& data)
{
	return WriteTextAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndian(const std::string& data)
{
	return WriteTextBigEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndian(const std::wstring& data)
{
	return WriteTextBigEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianAsUTF8(const std::string& data)
{
	return WriteTextBigEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianAsUTF8(const std::wstring& data)
{
	return WriteTextBigEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianAsUTF16(const std::string& data)
{
	return WriteTextBigEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianAsUTF16(const std::wstring& data)
{
	return WriteTextBigEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianAsUTF32(const std::string& data)
{
	return WriteTextBigEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianAsUTF32(const std::wstring& data)
{
	return WriteTextBigEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndian(const std::string& data)
{
	return WriteTextLittleEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndian(const std::wstring& data)
{
	return WriteTextLittleEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianAsUTF8(const std::string& data)
{
	return WriteTextLittleEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianAsUTF8(const std::wstring& data)
{
	return WriteTextLittleEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianAsUTF16(const std::string& data)
{
	return WriteTextLittleEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianAsUTF16(const std::wstring& data)
{
	return WriteTextLittleEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianAsUTF32(const std::string& data)
{
	return WriteTextLittleEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianAsUTF32(const std::wstring& data)
{
	return WriteTextLittleEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
//Fixed length text buffer write functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, memoryBuffer, codeUnitsInMemory, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsASCII(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF8(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF16(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF32(ByteOrder byteOrder, SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::LittleEndian:
		return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	case ByteOrder::BigEndian:
		return WriteTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data, paddingChar);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
//Data write functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, bool data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, char data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, signed char data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, unsigned char data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, wchar_t data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, short data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, unsigned short data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, int data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, unsigned int data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, long data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, unsigned long data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, long long data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, unsigned long long data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, float data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, double data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, long double data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Array write functions
//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const bool* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const char* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const signed char* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const unsigned char* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const wchar_t* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const short* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const unsigned short* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const int* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const unsigned int* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const unsigned long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const long long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const unsigned long long* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const float* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const double* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const long double* data, SizeType length)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data, length);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data, length);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteData(ByteOrder byteOrder, const std::vector<T>& data)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteData(data);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndian(data);
	case ByteOrder::BigEndian:
		return WriteDataBigEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteData(const std::vector<T>& data)
{
	SizeType length = (SizeType)data.size();
	if (length <= 0)
	{
		return true;
	}
	return WriteData(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataBigEndian(const std::vector<T>& data)
{
	SizeType length = (SizeType)data.size();
	if (length <= 0)
	{
		return true;
	}
	return WriteDataBigEndian(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataLittleEndian(const std::vector<T>& data)
{
	SizeType length = (SizeType)data.size();
	if (length <= 0)
	{
		return true;
	}
	return WriteDataLittleEndian(&data[0], length);
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteData(const std::vector<bool>& data)
{
	bool result = true;
	size_t elementCount = data.size();
	for (size_t i = 0; i < elementCount; ++i)
	{
		result |= WriteData(data[i]);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteDataBigEndian(const std::vector<bool>& data)
{
	bool result = true;
	size_t elementCount = data.size();
	for (size_t i = 0; i < elementCount; ++i)
	{
		result |= WriteDataBigEndian(data[i]);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool IStreamNonSeekable::WriteDataLittleEndian(const std::vector<bool>& data)
{
	bool result = true;
	size_t elementCount = data.size();
	for (size_t i = 0; i < elementCount; ++i)
	{
		result |= WriteDataLittleEndian(data[i]);
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataMultipleTimes(ByteOrder byteOrder, T data, SizeType count)
{
	switch (byteOrder)
	{
	case ByteOrder::Platform:
		return WriteDataMultipleTimes(data, count);
	case ByteOrder::LittleEndian:
		return WriteDataLittleEndianMultipleTimes(data, count);
	case ByteOrder::BigEndian:
		return WriteDataBigEndianMultipleTimes(data, count);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for (SizeType i = 0; i < count; ++i)
	{
		result &= WriteData(data);
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataBigEndianMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for (SizeType i = 0; i < count; ++i)
	{
		result &= WriteDataBigEndian(data);
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataLittleEndianMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for (SizeType i = 0; i < count; ++i)
	{
		result &= WriteDataLittleEndian(data);
	}
	return result;
}

} //Close namespace Stream
