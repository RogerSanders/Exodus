namespace Stream {

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IStream::ByteOrder
{
	BYTEORDER_PLATFORM,
	BYTEORDER_LITTLEENDIAN,
	BYTEORDER_BIGENDIAN
};

//----------------------------------------------------------------------------------------
enum IStream::TextEncoding
{
	TEXTENCODING_ASCII,
	TEXTENCODING_UTF8,
	TEXTENCODING_UTF16,
	TEXTENCODING_UTF32
};

//----------------------------------------------------------------------------------------
enum IStream::NewLineEncoding
{
	NEWLINEENCODING_UNIX,
	NEWLINEENCODING_DOS
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IStream::UnicodeCodePoint
{
	wchar_t codeUnit1;
	wchar_t codeUnit2;
	bool surrogatePair;
};

//----------------------------------------------------------------------------------------
//Fixed length text buffer read functions
//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::string& data, char paddingChar)
{
	data.resize((size_t)(codeUnitsInStream*2)+1);
	SizeType codeUnitsWritten = 0;
	bool result = ReadTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, &data[0], (SizeType)data.size(), codeUnitsWritten, paddingChar);
	data.resize((size_t)codeUnitsWritten);
	return result;
}

//----------------------------------------------------------------------------------------
bool IStream::ReadTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, std::wstring& data, wchar_t paddingChar)
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
template<class T> bool IStream::ReadData(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if(length <= 0)
	{
		return true;
	}
	return ReadData(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStream::ReadDataBigEndian(std::vector<T>& data, SizeType length)
{
	data.resize((std::vector<T>::size_type)length);
	if(length <= 0)
	{
		return true;
	}
	return ReadDataBigEndian(&data[0], length);
}

//----------------------------------------------------------------------------------------
template<class T> bool IStream::ReadDataLittleEndian(std::vector<T>& data, SizeType length)
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
bool IStream::WriteText(const std::string& data)
{
	return WriteText(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteText(const std::wstring& data)
{
	return WriteText(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextAsUTF8(const std::string& data)
{
	return WriteTextAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextAsUTF8(const std::wstring& data)
{
	return WriteTextAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextAsUTF16(const std::string& data)
{
	return WriteTextAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextAsUTF16(const std::wstring& data)
{
	return WriteTextAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextAsUTF32(const std::string& data)
{
	return WriteTextAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextAsUTF32(const std::wstring& data)
{
	return WriteTextAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndian(const std::string& data)
{
	return WriteTextBigEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndian(const std::wstring& data)
{
	return WriteTextBigEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianAsUTF8(const std::string& data)
{
	return WriteTextBigEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianAsUTF8(const std::wstring& data)
{
	return WriteTextBigEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianAsUTF16(const std::string& data)
{
	return WriteTextBigEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianAsUTF16(const std::wstring& data)
{
	return WriteTextBigEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianAsUTF32(const std::string& data)
{
	return WriteTextBigEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianAsUTF32(const std::wstring& data)
{
	return WriteTextBigEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndian(const std::string& data)
{
	return WriteTextLittleEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndian(const std::wstring& data)
{
	return WriteTextLittleEndian(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianAsUTF8(const std::string& data)
{
	return WriteTextLittleEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianAsUTF8(const std::wstring& data)
{
	return WriteTextLittleEndianAsUTF8(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianAsUTF16(const std::string& data)
{
	return WriteTextLittleEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianAsUTF16(const std::wstring& data)
{
	return WriteTextLittleEndianAsUTF16(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianAsUTF32(const std::string& data)
{
	return WriteTextLittleEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianAsUTF32(const std::wstring& data)
{
	return WriteTextLittleEndianAsUTF32(data.c_str(), (SizeType)data.length());
}

//----------------------------------------------------------------------------------------
//Fixed length text buffer write functions
//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextBigEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextBigEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsASCII(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsASCII(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsUTF8(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF8(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsUTF16(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF16(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::string& data, char paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
bool IStream::WriteTextLittleEndianFixedLengthBufferAsUTF32(SizeType codeUnitsInStream, const std::wstring& data, wchar_t paddingChar)
{
	return WriteTextLittleEndianFixedLengthBufferAsUTF32(codeUnitsInStream, data.c_str(), (SizeType)data.size(), paddingChar);
}

//----------------------------------------------------------------------------------------
//Array write functions
//----------------------------------------------------------------------------------------
template<class T> bool IStream::WriteData(const std::vector<T>& data)
{
	return WriteData(&data[0], (SizeType)data.size());
}

//----------------------------------------------------------------------------------------
template<class T> bool IStream::WriteDataMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for(SizeType i = 0; i < count; ++i)
	{
		result &= WriteData(data);
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStream::WriteDataBigEndian(const std::vector<T>& data)
{
	return WriteDataBigEndian(&data[0], (SizeType)data.size());
}

//----------------------------------------------------------------------------------------
template<class T> bool IStream::WriteDataBigEndianMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for(SizeType i = 0; i < count; ++i)
	{
		result &= WriteDataBigEndian(data);
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool IStream::WriteDataLittleEndian(const std::vector<T>& data)
{
	return WriteDataLittleEndian(&data[0], (SizeType)data.size());
}

//----------------------------------------------------------------------------------------
template<class T> bool IStream::WriteDataLittleEndianMultipleTimes(T data, SizeType count)
{
	bool result = true;
	for(SizeType i = 0; i < count; ++i)
	{
		result &= WriteDataLittleEndian(data);
	}
	return result;
}

} //Close namespace Stream
