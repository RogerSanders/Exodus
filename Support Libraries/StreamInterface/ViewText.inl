namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewText::ViewText(IStream& astream)
:stream(astream)
{}

//----------------------------------------------------------------------------------------
//Text-based access functions
//----------------------------------------------------------------------------------------
template<class T> bool ViewText::Read(T& data)
{
	//Initialize the formatted stream
	wcharStream.str(L"");

	//Record the initial stream position
	IStream::SizeType startStreamPos = stream.GetStreamPos();

	//Buffer text from the stream until we hit the end of the stream, or we reach a
	//newline or null terminator.
	bool done = false;
	while(!done && !stream.IsAtEnd())
	{
		IStream::UnicodeCodePoint codePoint;
		if(!stream.ReadChar(codePoint))
		{
			return false;
		}
		if(!codePoint.surrogatePair && ((codePoint.codeUnit1 == L'\0') || (codePoint.codeUnit1 == L'\n')))
		{
			done = true;
			continue;
		}
		//##FIX## Use a stream provided conversion function
		wcharStream << codePoint.codeUnit1;
		if(codePoint.surrogatePair)
		{
			wcharStream << codePoint.codeUnit2;
		}
	}

	//Perform a formatted read of the buffered data, using the specified type.
	wcharStream >> data;

	//Restore the stream position back to the initial state. We do this because some of
	//the data we read might not have actually been used.
	stream.SetStreamPos(startStreamPos);

	//Advance the stream forward by the actual number of characters that were used
	IStream::SizeType charsInData = (IStream::SizeType)wcharStream.tellg();
	for(IStream::SizeType i = 0; i < charsInData; ++i)
	{
		IStream::UnicodeCodePoint codePoint;
		if(!stream.ReadChar(codePoint))
		{
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(std::string& data)
{
	return ReadTextString(data, true);
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(std::wstring& data)
{
	return ReadTextString(data, true);
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(char& data)
{
	IStream::UnicodeCodePoint codePoint;
	if(!stream.ReadChar(codePoint))
	{
		return false;
	}
	//##FIX## Use a stream provided conversion function
	data = (char)codePoint.codeUnit1;
	return true;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(wchar_t& data)
{
	IStream::UnicodeCodePoint codePoint;
	if(!stream.ReadChar(codePoint))
	{
		return false;
	}
	if(codePoint.surrogatePair)
	{
		return false;
	}
	//##FIX## Use a stream provided conversion function
	data = codePoint.codeUnit1;
	return true;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(signed char& data)
{
	int temp;
	bool result = Read(temp);
	data = (signed char)temp;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(unsigned char& data)
{
	unsigned int temp;
	bool result = Read(temp);
	data = (unsigned char)temp;
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool ViewText::Write(const T& data)
{
	wcharStream.str(L"");
	wcharStream << data;
	return stream.WriteText(wcharStream.str());
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const float& data)
{
	wcharStream.str(L"");
	wcharStream << std::fixed << std::setprecision(7) << data;
	return stream.WriteText(wcharStream.str());
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const double& data)
{
	wcharStream.str(L"");
	wcharStream << std::fixed << std::setprecision(16) << data;
	return stream.WriteText(wcharStream.str());
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const long double& data)
{
	wcharStream.str(L"");
	wcharStream << std::fixed << std::setprecision(64) << data;
	return stream.WriteText(wcharStream.str());
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const std::string& data)
{
	return stream.WriteText(data);
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const std::wstring& data)
{
	return stream.WriteText(data);
}

//----------------------------------------------------------------------------------------
//##FIX## These functions are unsafe
template<> bool ViewText::Write(char* const& data)
{
	return stream.WriteText(data, GetStringLength(data));
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(wchar_t* const& data)
{
	return stream.WriteText(data, GetStringLength(data));
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const char* const& data)
{
	return stream.WriteText(data, GetStringLength(data));
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const wchar_t* const& data)
{
	return stream.WriteText(data, GetStringLength(data));
}

//----------------------------------------------------------------------------------------
template<size_t S> bool ViewText::Write(const char(&data)[S])
{
	return stream.WriteText(&data[0], GetStringLength(data));
}

//----------------------------------------------------------------------------------------
template<size_t S> bool ViewText::Write(const wchar_t(&data)[S])
{
	return stream.WriteText(&data[0], GetStringLength(data));
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const char& data)
{
	std::string temp;
	temp.push_back(data);
	return Write(temp);
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const wchar_t& data)
{
	std::wstring temp;
	temp.push_back(data);
	return Write(temp);
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const signed char& data)
{
	int temp = data;
	return Write(temp);
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const unsigned char& data)
{
	unsigned int temp = data;
	return Write(temp);
}

//----------------------------------------------------------------------------------------
//Text string read functions
//----------------------------------------------------------------------------------------
bool ViewText::ReadTextString(std::string& data, bool stopAtNewline)
{
	//Read text from the stream until we hit the end of the stream, or we reach a newline
	//or null terminator.
	data.clear();
	bool done = false;
	while(!done && !stream.IsAtEnd())
	{
		IStream::UnicodeCodePoint codePoint;
		if(!stream.ReadChar(codePoint))
		{
			return false;
		}
		if(!codePoint.surrogatePair && ((codePoint.codeUnit1 == L'\0') || (codePoint.codeUnit1 == L'\n')))
		{
			done = true;
			continue;
		}
		//##FIX## Use a stream provided conversion function
		data.push_back((char)codePoint.codeUnit1);
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewText::ReadTextString(std::wstring& data, bool stopAtNewline)
{
	//Read text from the stream until we hit the end of the stream, or we reach a newline
	//or null terminator.
	data.clear();
	bool done = false;
	while(!done && !stream.IsAtEnd())
	{
		IStream::UnicodeCodePoint codePoint;
		if(!stream.ReadChar(codePoint))
		{
			return false;
		}
		if(!codePoint.surrogatePair && ((codePoint.codeUnit1 == L'\0') || (stopAtNewline && (codePoint.codeUnit1 == L'\n'))))
		{
			done = true;
			continue;
		}
		//##FIX## Use a stream provided conversion function
		data.push_back(codePoint.codeUnit1);
		if(codePoint.surrogatePair)
		{
			data.push_back(codePoint.codeUnit2);
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Text stream functions
//----------------------------------------------------------------------------------------
template<class T> ViewText& ViewText::operator>>(T& data)
{
	Read(data);
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> ViewText& ViewText::operator<<(const T& data)
{
	Write(data);
	return *this;
}

//----------------------------------------------------------------------------------------
//String length functions
//----------------------------------------------------------------------------------------
IStream::SizeType ViewText::GetStringLength(const char* data)
{
	IStream::SizeType size = 0;
	while(data[size] != '\0')
	{
		++size;
	}
	return size;
}

//----------------------------------------------------------------------------------------
IStream::SizeType ViewText::GetStringLength(const wchar_t* data)
{
	IStream::SizeType size = 0;
	while(data[size] != L'\0')
	{
		++size;
	}
	return size;
}

} //Close namespace Stream
