#include <iomanip>
namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewText::ViewText(IStream& astream)
:stream(astream), byteOrder(IStream::ByteOrder::Platform), noErrorState(true)
{}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
bool ViewText::NoErrorsOccurred() const
{
	return noErrorState;
}

//----------------------------------------------------------------------------------------
void ViewText::ClearErrorState()
{
	noErrorState = true;
}

//----------------------------------------------------------------------------------------
bool ViewText::IsAtEnd() const
{
	return stream.IsAtEnd();
}

//----------------------------------------------------------------------------------------
IStream::ByteOrder ViewText::GetViewByteOrder() const
{
	return byteOrder;
}

//----------------------------------------------------------------------------------------
void ViewText::SetViewByteOrder(IStream::ByteOrder abyteOrder)
{
	byteOrder = abyteOrder;
}

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
		if(!stream.ReadChar(byteOrder, codePoint))
		{
			noErrorState = false;
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
		if(!stream.ReadChar(byteOrder, codePoint))
		{
			noErrorState = false;
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(bool& data)
{
	const wchar_t* trueString = L"true";
	const wchar_t* falseString = L"false";
	const unsigned int trueStringLength = 4;
	const unsigned int falseStringLength = 5;
	unsigned int stringCharIndex = 0;
	bool readingFalseString = false;
	bool result = false;
	bool completedRead = false;
	while(!completedRead)
	{
		//Attempt to read the next character from the stream
		IStream::UnicodeCodePoint codePoint;
		if(!stream.ReadChar(byteOrder, codePoint))
		{
			completedRead = true;
		}

		//If this character defined a surrogate pair, it definitely isn't one we support.
		//Return false in this case.
		if(codePoint.surrogatePair)
		{
			completedRead = true;
		}

		//If this is the first character we've tried to read, and it defines either a 0 or
		//a 1, convert the numeric value into a boolean value.
		if((stringCharIndex == 0) && ((codePoint.codeUnit1 == L'0') || (codePoint.codeUnit1 == L'1')))
		{
			data = (codePoint.codeUnit1 == L'1');
			result = true;
			completedRead = true;
		}

		//If this character appears to be part of a string representation of a boolean,
		//read and validate the next character from the string. If we reach the end of the
		//string, set the data value to the appropriate value, and return true.
		if((!readingFalseString || (stringCharIndex == 0)) && (trueString[stringCharIndex] == (wchar_t)tolower((int)codePoint.codeUnit1)))
		{
			readingFalseString = false;
			++stringCharIndex;
			if(stringCharIndex == trueStringLength)
			{
				data = true;
				result = true;
				completedRead = true;
			}
			continue;
		}
		if((readingFalseString || (stringCharIndex == 0)) && (falseString[stringCharIndex] == (wchar_t)tolower((int)codePoint.codeUnit1)))
		{
			readingFalseString = true;
			++stringCharIndex;
			if(stringCharIndex == falseStringLength)
			{
				data = true;
				result = true;
				completedRead = true;
			}
			continue;
		}

		//If we get to this point, an invalid character was encountered, so we return
		//false.
		completedRead = true;
	}
	if(!result)
	{
		noErrorState = true;
	}
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(std::string& data)
{
	bool result = ReadTextString(data, true);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(std::wstring& data)
{
	bool result = ReadTextString(data, true);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(char& data)
{
	IStream::UnicodeCodePoint codePoint;
	if(!stream.ReadChar(byteOrder, codePoint))
	{
		noErrorState = false;
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
	if(!stream.ReadChar(byteOrder, codePoint))
	{
		noErrorState = false;
		return false;
	}
	if(codePoint.surrogatePair)
	{
		noErrorState = false;
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
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Read(unsigned char& data)
{
	unsigned int temp;
	bool result = Read(temp);
	data = (unsigned char)temp;
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<class T> bool ViewText::Write(const T& data)
{
	wcharStream.str(L"");
	wcharStream << data;
	bool result = stream.WriteText(byteOrder, wcharStream.str());
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const bool& data)
{
	wchar_t* boolAsString = data? L"1": L"0";
	bool result = stream.WriteText(byteOrder, boolAsString);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const float& data)
{
	wcharStream.str(L"");
	wcharStream << std::fixed << std::setprecision(7) << data;
	bool result = stream.WriteText(byteOrder, wcharStream.str());
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const double& data)
{
	wcharStream.str(L"");
	wcharStream << std::fixed << std::setprecision(16) << data;
	bool result = stream.WriteText(byteOrder, wcharStream.str());
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const long double& data)
{
	wcharStream.str(L"");
	wcharStream << std::fixed << std::setprecision(64) << data;
	bool result = stream.WriteText(byteOrder, wcharStream.str());
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const std::string& data)
{
	bool result = stream.WriteText(byteOrder, data);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const std::wstring& data)
{
	bool result = stream.WriteText(byteOrder, data);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
//##FIX## These functions are unsafe
template<> bool ViewText::Write(char* const& data)
{
	bool result = stream.WriteText(byteOrder, data, GetStringLength(data));
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(wchar_t* const& data)
{
	bool result = stream.WriteText(byteOrder, data, GetStringLength(data));
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const char* const& data)
{
	bool result = stream.WriteText(byteOrder, data, GetStringLength(data));
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const wchar_t* const& data)
{
	bool result = stream.WriteText(byteOrder, data, GetStringLength(data));
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<size_t S> bool ViewText::Write(const char(&data)[S])
{
	IStream::SizeType stringLength = GetStringLength(data, S);
	if(stringLength <= 0)
	{
		return true;
	}
	bool result = stream.WriteText(byteOrder, &data[0], stringLength);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<size_t S> bool ViewText::Write(const wchar_t(&data)[S])
{
	IStream::SizeType stringLength = GetStringLength(data, S);
	if(stringLength <= 0)
	{
		return true;
	}
	bool result = stream.WriteText(byteOrder, &data[0], stringLength);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const char& data)
{
	std::string temp;
	temp.push_back(data);
	bool result = Write(temp);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const wchar_t& data)
{
	std::wstring temp;
	temp.push_back(data);
	bool result = Write(temp);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const signed char& data)
{
	int temp = data;
	bool result = Write(temp);
	noErrorState &= result;
	return result;
}

//----------------------------------------------------------------------------------------
template<> bool ViewText::Write(const unsigned char& data)
{
	unsigned int temp = data;
	bool result = Write(temp);
	noErrorState &= result;
	return result;
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
		if(!stream.ReadChar(byteOrder, codePoint))
		{
			noErrorState = false;
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
		if(!stream.ReadChar(byteOrder, codePoint))
		{
			noErrorState = false;
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

//----------------------------------------------------------------------------------------
IStream::SizeType ViewText::GetStringLength(const char* data, size_t bufferSize)
{
	IStream::SizeType size = 0;
	while((size < (IStream::SizeType)bufferSize) && (data[size] != '\0'))
	{
		++size;
	}
	return size;
}

//----------------------------------------------------------------------------------------
IStream::SizeType ViewText::GetStringLength(const wchar_t* data, size_t bufferSize)
{
	IStream::SizeType size = 0;
	while((size < (IStream::SizeType)bufferSize) && (data[size] != L'\0'))
	{
		++size;
	}
	return size;
}

} //Close namespace Stream
