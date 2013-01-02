namespace Stream {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IStreamNonSeekable::ByteOrder
{
	BYTEORDER_PLATFORM,
	BYTEORDER_LITTLEENDIAN,
	BYTEORDER_BIGENDIAN
};

//----------------------------------------------------------------------------------------
enum IStreamNonSeekable::TextEncoding
{
	TEXTENCODING_ASCII,
	TEXTENCODING_UTF8,
	TEXTENCODING_UTF16,
	TEXTENCODING_UTF32
};

//----------------------------------------------------------------------------------------
enum IStreamNonSeekable::NewLineEncoding
{
	NEWLINEENCODING_UNIX,
	NEWLINEENCODING_DOS
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
//Fixed length text buffer read functions
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
//Array read functions
//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::ReadData(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if(length <= 0)
	{
		return true;
	}
	return ReadData(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::ReadDataBigEndian(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if(length <= 0)
	{
		return true;
	}
	return ReadDataBigEndian(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::ReadDataLittleEndian(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if(length <= 0)
	{
		return true;
	}
	return ReadDataLittleEndian(&data[0], length);
}

//----------------------------------------------------------------------------------------
//Text string write functions
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
//Array write functions
//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteData(const std::vector<T>& data)
{
	return WriteData(&data[0], (SizeType)data.size());
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for(SizeType i = 0; i < count; ++i)
	{
		result &= WriteData(data);
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataBigEndian(const std::vector<T>& data)
{
	return WriteDataBigEndian(&data[0], (SizeType)data.size());
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataBigEndianMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for(SizeType i = 0; i < count; ++i)
	{
		result &= WriteDataBigEndian(data);
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataLittleEndian(const std::vector<T>& data)
{
	return WriteDataLittleEndian(&data[0], (SizeType)data.size());
}

//----------------------------------------------------------------------------------------
template<class T> bool IStreamNonSeekable::WriteDataLittleEndianMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for(SizeType i = 0; i < count; ++i)
	{
		result &= WriteDataLittleEndian(data);
	}
	return result;
}

} //Close namespace Stream
