namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Stream::Stream(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder)
:byteOrder(abyteOrder), newLineEncoding(anewLineEncoding)
{
	SetTextEncoding(atextEncoding);
}

//----------------------------------------------------------------------------------------
//Internal text format independent char read functions
//----------------------------------------------------------------------------------------
bool Stream::ReadCharInternal(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn)
{
	switch(textEncoding)
	{
	case TEXTENCODING_ASCII:
		return ReadCharInternalAsASCII(data, byteOrder, remainingCodeUnitsAvailable, stripCarriageReturn);
	case TEXTENCODING_UTF8:
		return ReadCharInternalAsUTF8(data, byteOrder, remainingCodeUnitsAvailable, stripCarriageReturn);
	case TEXTENCODING_UTF16:
		return ReadCharInternalAsUTF16(data, byteOrder, remainingCodeUnitsAvailable, stripCarriageReturn);
	case TEXTENCODING_UTF32:
		return ReadCharInternalAsUTF32(data, byteOrder, remainingCodeUnitsAvailable, stripCarriageReturn);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharInternalAsASCII(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn)
{
	bool result = true;
	do
	{
		//Ensure that code units are available in the stream
		if(remainingCodeUnitsAvailable <= 0)
		{
			result = false;
			continue;
		}

		//##TODO## Consider if we should be excluding characters above 0x7F
		unsigned char codeUnit;
		result &= ReadDataInternal(codeUnit, abyteOrder);
		--remainingCodeUnitsAvailable;
		ConvertASCIIToUnicodeCodePoint(codeUnit, data);
	} while(result && stripCarriageReturn && (data.codeUnit1 == L'\r'));
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharInternalAsUTF8(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn)
{
	bool result = true;

	bool done = false;
	do
	{
		//Ensure that code units are available in the stream
		if(remainingCodeUnitsAvailable <= 0)
		{
			result = false;
			continue;
		}

		unsigned int codePoint = 0;

		//Read in the first code unit
		unsigned char codeUnit1;
		result &= ReadDataInternal(codeUnit1, abyteOrder);
		--remainingCodeUnitsAvailable;

		//If this a continuation byte, the data is invalid. Abort the read.
		if((codeUnit1 & 0xC0) == 0x80)
		{
			result = false;
			continue;
		}

		//Determine how many continuation bytes to read
		unsigned int continuationBytesToRead = 0;
		if((codeUnit1 & 0x80) == 0x00)
		{
			codePoint = codeUnit1 & 0x7F;
		}
		else
		{
			switch((codeUnit1 >> 3) & 0x07)
			{
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
				continuationBytesToRead = 1;
				codePoint = codeUnit1 & 0x1F;
				break;
			case 0x04:
			case 0x05:
				continuationBytesToRead = 2;
				codePoint = codeUnit1 & 0x0F;
				break;
			case 0x06:
				continuationBytesToRead = 3;
				codePoint = codeUnit1 & 0x07;
				break;
			case 0x07:
				//This code unit indicates too many continuation bytes. Abort the read.
				done = true;
				result = false;
				continue;
			}
		}

		//Read all the continuation bytes
		for(unsigned int i = 0; result && (i < continuationBytesToRead); ++i)
		{
			//Ensure that code units are available in the stream
			if(remainingCodeUnitsAvailable <= 0)
			{
				result = false;
				continue;
			}

			//Read in the next continuation byte
			unsigned char continuationByte;
			result &= ReadDataInternal(continuationByte, abyteOrder);
			--remainingCodeUnitsAvailable;

			//Ensure this byte is correctly tagged as a continuation byte
			if((continuationByte & 0xC0) != 0x80)
			{
				result = false;
				continue;
			}

			//Combine this continuation byte with the code point
			codePoint = (codePoint << 6) | (continuationByte & 0x3F);
		}

		//If there was an error processing the continuation bytes, abort.
		if(!result)
		{
			continue;
		}

		//Ensure the code point doesn't represent a character in the reserved UTF16 range
		if((codePoint & 0xFFFFF800) == 0x0000D800)
		{
			result = false;
			continue;
		}

		//Ensure the code point comes from a valid code point plane. We do this to ensure
		//that there is a valid conversion to other supported unicode formats.
		if(codePoint > 0x0010FFFF)
		{
			result = false;
			continue;
		}

		//Convert from UTF32 to native encoding
		ConvertUTF32ToUnicodeCodePoint(codePoint, data);

		//If we've encountered a carriage return, skip it.
		if(!data.surrogatePair && stripCarriageReturn && (data.codeUnit1 == L'\r'))
		{
			continue;
		}

		//We're done
		done = true;
	} while(!done && result);

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharInternalAsUTF16(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn)
{
	bool result = true;

	bool done = false;
	do
	{
		//Ensure that code units are available in the stream
		if(remainingCodeUnitsAvailable <= 0)
		{
			result = false;
			continue;
		}

		//Read in the first code unit
		unsigned short codeUnit1;
		result &= ReadDataInternal(codeUnit1, abyteOrder);
		--remainingCodeUnitsAvailable;

		//If this is the second half of a surrogate pair, the data is invalid. Abort the
		//read.
		if((codeUnit1 & 0xFC00) == 0xDC00)
		{
			result = false;
			continue;
		}

		//If this is the first half of a surrogate pair, read in the second half.
		bool surrogatePair = false;
		unsigned short codeUnit2 = 0;
		if((codeUnit1 & 0xFC00) == 0xD800)
		{
			//Ensure that code units are available in the stream
			if(remainingCodeUnitsAvailable <= 0)
			{
				result = false;
				continue;
			}

			//Read in the second code unit
			result &= ReadDataInternal(codeUnit2, abyteOrder);
			--remainingCodeUnitsAvailable;
			surrogatePair = true;

			//If this is isn't the second half of a surrogate pair, the data is invalid.
			//Abort the read.
			if((codeUnit2 & 0xFC00) != 0xDC00)
			{
				result = false;
				continue;
			}
		}

		//If we failed to read in the second code unit, abort.
		if(!result)
		{
			continue;
		}

		//Convert from UTF16 to native encoding
		ConvertUTF16ToUnicodeCodePoint(codeUnit1, codeUnit2, surrogatePair, data);

		//If we've encountered a carriage return, skip it.
		if(!data.surrogatePair && stripCarriageReturn && (data.codeUnit1 == L'\r'))
		{
			continue;
		}

		//We're done
		done = true;
	} while(!done && result);

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadCharInternalAsUTF32(UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool stripCarriageReturn)
{
	bool result = true;

	bool done = false;
	do
	{
		//Ensure that code units are available in the stream
		if(remainingCodeUnitsAvailable <= 0)
		{
			result = false;
			continue;
		}

		//Read in the code point
		unsigned int codePoint;
		result &= ReadDataInternal(&codePoint, abyteOrder);
		--remainingCodeUnitsAvailable;

		//If we failed to read in the code point, abort.
		if(!result)
		{
			continue;
		}

		//Ensure the code point doesn't represent a character in the reserved UTF16 range
		if((codePoint & 0xFFFFF800) == 0x0000D800)
		{
			result = false;
			continue;
		}

		//Ensure the code point comes from a valid code point plane. We do this to ensure
		//that there is a valid conversion to other supported unicode formats.
		if(codePoint > 0x0010FFFF)
		{
			result = false;
			continue;
		}

		//Convert from UTF32 to native encoding
		ConvertUTF32ToUnicodeCodePoint(codePoint, data);

		//If we've encountered a carriage return, skip it.
		if(!data.surrogatePair && stripCarriageReturn && (data.codeUnit1 == L'\r'))
		{
			continue;
		}

		//Now that we've read in the code point, we're done.
		done = true;
	} while(!done && result);

	return result;
}

//----------------------------------------------------------------------------------------
//Internal fixed length text buffer read functions
//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsASCII(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to ASCII
		char codeUnit;
		result &= ConvertUnicodeCodePointToChar(codePoint, codeUnit);

		if(codeUnit == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			*(memoryBuffer + codeUnitsWritten) = codeUnit;
			++codeUnitsWritten;
		}
	}
	*(memoryBuffer + codeUnitsWritten) = '\0';
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsASCII(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to wchar_t
		const SizeType bufferSize = 6;
		wchar_t codeUnits[bufferSize];
		SizeType codeUnitsConverted = 0;
		result &= ConvertUnicodeCodePointToWCharT(codePoint, &codeUnits[0], bufferSize, codeUnitsConverted);

		if(codeUnits[0] == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			if((codeUnitsWritten + codeUnitsConverted) > codeUnitsInMemory)
			{
				result = false;
			}
			else
			{
				for(unsigned int codeUnitIndex = 0; codeUnitIndex < codeUnitsConverted; ++codeUnitIndex)
				{
					*(memoryBuffer + codeUnitsWritten) = codeUnits[codeUnitIndex];
					++codeUnitsWritten;
				}
			}
		}
	}
	*(memoryBuffer + codeUnitsWritten) = L'\0';
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsUTF8(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to ASCII
		char codeUnit;
		result &= ConvertUnicodeCodePointToChar(codePoint, codeUnit);

		if(codeUnit == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			*(memoryBuffer + codeUnitsWritten) = codeUnit;
			++codeUnitsWritten;
		}
	}
	*(memoryBuffer + codeUnitsWritten) = '\0';
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsUTF8(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to wchar_t
		const SizeType bufferSize = 6;
		wchar_t codeUnits[bufferSize];
		SizeType codeUnitsConverted = 0;
		result &= ConvertUnicodeCodePointToWCharT(codePoint, &codeUnits[0], bufferSize, codeUnitsConverted);

		if(codeUnits[0] == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			if((codeUnitsWritten + codeUnitsConverted) > codeUnitsInMemory)
			{
				result = false;
			}
			else
			{
				for(unsigned int codeUnitIndex = 0; codeUnitIndex < codeUnitsConverted; ++codeUnitIndex)
				{
					*(memoryBuffer + codeUnitsWritten) = codeUnits[codeUnitIndex];
					++codeUnitsWritten;
				}
			}
		}
	}
	*(memoryBuffer + codeUnitsWritten) = L'\0';
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsUTF16(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to ASCII
		char codeUnit;
		result &= ConvertUnicodeCodePointToChar(codePoint, codeUnit);

		if(codeUnit == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			*(memoryBuffer + codeUnitsWritten) = codeUnit;
			++codeUnitsWritten;
		}
	}
	*(memoryBuffer + codeUnitsWritten) = '\0';
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsUTF16(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to wchar_t
		const SizeType bufferSize = 6;
		wchar_t codeUnits[bufferSize];
		SizeType codeUnitsConverted = 0;
		result &= ConvertUnicodeCodePointToWCharT(codePoint, &codeUnits[0], bufferSize, codeUnitsConverted);

		if(codeUnits[0] == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			if((codeUnitsWritten + codeUnitsConverted) > codeUnitsInMemory)
			{
				result = false;
			}
			else
			{
				for(unsigned int codeUnitIndex = 0; codeUnitIndex < codeUnitsConverted; ++codeUnitIndex)
				{
					*(memoryBuffer + codeUnitsWritten) = codeUnits[codeUnitIndex];
					++codeUnitsWritten;
				}
			}
		}
	}
	*(memoryBuffer + codeUnitsWritten) = L'\0';
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, char* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, char paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsUTF32(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to ASCII
		char codeUnit;
		result &= ConvertUnicodeCodePointToChar(codePoint, codeUnit);

		if(codeUnit == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			*(memoryBuffer + codeUnitsWritten) = codeUnit;
			++codeUnitsWritten;
		}
	}
	*(memoryBuffer + codeUnitsWritten) = '\0';
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::ReadTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, wchar_t* memoryBuffer, SizeType codeUnitsInMemory, SizeType& codeUnitsWritten, wchar_t paddingChar)
{
	bool result = true;
	bool foundEndOfString = false;
	codeUnitsWritten = 0;
	while(codeUnitsInStream > 0)
	{
		UnicodeCodePoint codePoint;
		result &= ReadCharInternalAsUTF32(codePoint, byteOrder, codeUnitsInStream, false);

		//Convert from UnicodeCodePoint to wchar_t
		const SizeType bufferSize = 6;
		wchar_t codeUnits[bufferSize];
		SizeType codeUnitsConverted = 0;
		result &= ConvertUnicodeCodePointToWCharT(codePoint, &codeUnits[0], bufferSize, codeUnitsConverted);

		if(codeUnits[0] == paddingChar)
		{
			foundEndOfString = true;
		}
		if(!foundEndOfString)
		{
			if((codeUnitsWritten + codeUnitsConverted) > codeUnitsInMemory)
			{
				result = false;
			}
			else
			{
				for(unsigned int codeUnitIndex = 0; codeUnitIndex < codeUnitsConverted; ++codeUnitIndex)
				{
					*(memoryBuffer + codeUnitsWritten) = codeUnits[codeUnitIndex];
					++codeUnitsWritten;
				}
			}
		}
	}
	*(memoryBuffer + codeUnitsWritten) = L'\0';
	return result;
}

//----------------------------------------------------------------------------------------
//Internal type-independent data write functions
//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternal(T& data, ByteOrder abyteOrder)
{
	switch(abyteOrder)
	{
	case BYTEORDER_PLATFORM:
		return ReadBinaryNativeByteOrder(data);
	case BYTEORDER_BIGENDIAN:
		return ReadDataInternalBigEndian(data);
	case BYTEORDER_LITTLEENDIAN:
		return ReadDataInternalLittleEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternal(T& data)
{
	switch(byteOrder)
	{
	case BYTEORDER_PLATFORM:
		return ReadBinaryNativeByteOrder(data);
	case BYTEORDER_BIGENDIAN:
		return ReadDataInternalBigEndian(data);
	case BYTEORDER_LITTLEENDIAN:
		return ReadDataInternalLittleEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternalBigEndian(T& data)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return ReadBinaryNativeByteOrder(data);
#else
	return ReadBinaryInvertedByteOrder(data);
#endif
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternalLittleEndian(T& data)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return ReadBinaryInvertedByteOrder(data);
#else
	return ReadBinaryNativeByteOrder(data);
#endif
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternal(T* data, SizeType length, ByteOrder abyteOrder)
{
	switch(abyteOrder)
	{
	case BYTEORDER_PLATFORM:
		return ReadBinaryNativeByteOrder(data, length);
	case BYTEORDER_BIGENDIAN:
		return ReadDataInternalBigEndian(data, length);
	case BYTEORDER_LITTLEENDIAN:
		return ReadDataInternalLittleEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternal(T* data, SizeType length)
{
	switch(byteOrder)
	{
	case BYTEORDER_PLATFORM:
		return ReadBinaryNativeByteOrder(data, length);
	case BYTEORDER_BIGENDIAN:
		return ReadDataInternalBigEndian(data, length);
	case BYTEORDER_LITTLEENDIAN:
		return ReadDataInternalLittleEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternalBigEndian(T* data, SizeType length)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return ReadBinaryNativeByteOrder(data, length);
#else
	return ReadBinaryInvertedByteOrder(data, length);
#endif
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadDataInternalLittleEndian(T* data, SizeType length)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return ReadBinaryInvertedByteOrder(data, length);
#else
	return ReadBinaryNativeByteOrder(data, length);
#endif
}

//----------------------------------------------------------------------------------------
//Inverted byte order read functions
//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadBinaryInvertedByteOrder(T& data)
{
	//Read the data in the native byte order
	T temp;
	if(!ReadBinaryNativeByteOrder(temp))
	{
		return false;
	}

	//Invert the byte order
	unsigned char* binaryData = (unsigned char*)&data;
	unsigned char* binaryTemp = (unsigned char*)&temp;
	for(unsigned int i = 0; i < sizeof(data); ++i)
	{
		*(binaryData + i) = *(binaryTemp + ((sizeof(data) - 1) - i));
	}

	return true;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::ReadBinaryInvertedByteOrder(T* data, SizeType length)
{
	//Read the data in the native byte order
	T* temp = new T[(size_t)length];
	if(!ReadBinaryNativeByteOrder(temp, length))
	{
		delete[] temp;
		return false;
	}

	//Invert the byte order
	unsigned int dataTypeSize = sizeof(*data);
	for(unsigned int entry = 0; entry < length; ++entry)
	{
		unsigned char* binaryData = (unsigned char*)&data[entry];
		unsigned char* binaryTemp = (unsigned char*)&temp[entry];
		for(unsigned int i = 0; i < dataTypeSize; ++i)
		{
			*(binaryData + i) = *(binaryTemp + ((dataTypeSize - 1) - i));
		}
	}

	delete[] temp;
	return true;
}

//----------------------------------------------------------------------------------------
//Internal text format independent char write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteCharInternal(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn)
{
	switch(textEncoding)
	{
	case TEXTENCODING_ASCII:
		return WriteCharInternalAsASCII(data, byteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	case TEXTENCODING_UTF8:
		return WriteCharInternalAsUTF8(data, byteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	case TEXTENCODING_UTF16:
		return WriteCharInternalAsUTF16(data, byteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	case TEXTENCODING_UTF32:
		return WriteCharInternalAsUTF32(data, byteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharInternalAsASCII(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn)
{
	bool result = true;

	//Check if we need to insert a carriage return
	if(insertCarriageReturn && (data.codeUnit1 == L'\n'))
	{
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint('\r', codePoint);
		result &= WriteCharInternalAsASCII(codePoint, abyteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	}

	//Ensure that code units are available in the stream
	if(remainingCodeUnitsAvailable <= 0)
	{
		return false;
	}

	//Convert to ASCII
	char codeUnit;
	result &= ConvertUnicodeCodePointToChar(data, codeUnit);

	//Write the data
	result &= WriteDataInternal(codeUnit, abyteOrder);
	--remainingCodeUnitsAvailable;

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharInternalAsUTF8(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn)
{
	bool result = true;

	//Check if we need to insert a carriage return
	if(insertCarriageReturn && (data.codeUnit1 == L'\n'))
	{
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint('\r', codePoint);
		result &= WriteCharInternalAsUTF8(codePoint, abyteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	}

	//Convert to UTF32 encoding
	unsigned int codePoint;
	ConvertUnicodeCodePointToUTF32(data, codePoint);

	//Determine how many bytes we need to write, and encode the data.
	unsigned int bytesToWrite;
	unsigned char codeUnits[6];
	if(codePoint > 0x0010FFFF)
	{
		return false;
	}
	else if(codePoint > 0x0000FFFF)
	{
		bytesToWrite = 4;
		codeUnits[0] = ((unsigned char)(codePoint >> 18) & 0x07) | 0xF0;
		codeUnits[1] = ((unsigned char)(codePoint >> 12) & 0x3F) | 0x80;
		codeUnits[2] = ((unsigned char)(codePoint >> 6) & 0x3F) | 0x80;
		codeUnits[3] = ((unsigned char)(codePoint >> 0) & 0x3F) | 0x80;
	}
	else if(codePoint > 0x000007FF)
	{
		bytesToWrite = 3;
		codeUnits[0] = ((unsigned char)(codePoint >> 12) & 0x0F) | 0xE0;
		codeUnits[1] = ((unsigned char)(codePoint >> 6) & 0x3F) | 0x80;
		codeUnits[2] = ((unsigned char)(codePoint >> 0) & 0x3F) | 0x80;
	}
	else if(codePoint > 0x0000007F)
	{
		bytesToWrite = 2;
		codeUnits[0] = ((unsigned char)(codePoint >> 6) & 0x1F) | 0xC0;
		codeUnits[1] = ((unsigned char)(codePoint >> 0) & 0x3F) | 0x80;
	}
	else
	{
		bytesToWrite = 1;
		codeUnits[0] = (unsigned char)codePoint;
	}

	//Ensure that code units are available in the stream
	if(remainingCodeUnitsAvailable < (SizeType)bytesToWrite)
	{
		return false;
	}

	//Write the data
	for(unsigned int i = 0; i < bytesToWrite; ++i)
	{
		result &= WriteDataInternal(codeUnits[i], abyteOrder);
		--remainingCodeUnitsAvailable;
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharInternalAsUTF16(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn)
{
	bool result = true;

	//Check if we need to insert a carriage return
	if(insertCarriageReturn && (data.codeUnit1 == L'\n'))
	{
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint('\r', codePoint);
		result &= WriteCharInternalAsUTF16(codePoint, abyteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	}

	//Convert to UTF16 encoding
	bool surrogatePair;
	unsigned short codeUnit1;
	unsigned short codeUnit2;
	ConvertUnicodeCodePointToUTF16(data, codeUnit1, codeUnit2, surrogatePair);

	//Ensure that code units are available in the stream
	SizeType codeUnitsToWrite = surrogatePair? 2: 1;
	if(remainingCodeUnitsAvailable < codeUnitsToWrite)
	{
		return false;
	}

	//Write the data
	result &= WriteDataInternal(codeUnit1, abyteOrder);
	--remainingCodeUnitsAvailable;
	if(surrogatePair)
	{
		result &= WriteDataInternal(codeUnit2, abyteOrder);
		--remainingCodeUnitsAvailable;
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteCharInternalAsUTF32(const UnicodeCodePoint& data, ByteOrder abyteOrder, SizeType& remainingCodeUnitsAvailable, bool insertCarriageReturn)
{
	bool result = true;

	//Check if we need to insert a carriage return
	if(insertCarriageReturn && (data.codeUnit1 == L'\n'))
	{
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint('\r', codePoint);
		result &= WriteCharInternalAsUTF32(codePoint, abyteOrder, remainingCodeUnitsAvailable, insertCarriageReturn);
	}

	//Convert to UTF32 encoding
	unsigned int codeUnit;
	ConvertUnicodeCodePointToUTF32(data, codeUnit);

	//Ensure that code units are available in the stream
	if(remainingCodeUnitsAvailable <= 0)
	{
		return false;
	}

	//Write the data
	result &= WriteDataInternal(codeUnit, abyteOrder);
	--remainingCodeUnitsAvailable;

	return result;
}

//----------------------------------------------------------------------------------------
//Text string write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternal(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertCharToUnicodeCodePoint(*(data + i), codePoint))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternal(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
		++i;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternal(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertWCharTToUnicodeCodePoint((data + i), codePoint, bufferSize, i))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternal(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsASCII(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertCharToUnicodeCodePoint(*(data + i), codePoint))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsASCII(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
		++i;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsASCII(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertWCharTToUnicodeCodePoint((data + i), codePoint, bufferSize, i))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsASCII(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsUTF8(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertCharToUnicodeCodePoint(*(data + i), codePoint))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsUTF8(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
		++i;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsUTF8(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertWCharTToUnicodeCodePoint((data + i), codePoint, bufferSize, i))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsUTF8(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsUTF16(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertCharToUnicodeCodePoint(*(data + i), codePoint))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsUTF16(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
		++i;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsUTF16(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertWCharTToUnicodeCodePoint((data + i), codePoint, bufferSize, i))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsUTF16(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsUTF32(ByteOrder abyteOrder, const char* data, SizeType bufferSize, char terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertCharToUnicodeCodePoint(*(data + i), codePoint))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsUTF32(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
		++i;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalAsUTF32(ByteOrder abyteOrder, const wchar_t* data, SizeType bufferSize, wchar_t terminator)
{
	bool done = false;
	bool result = true;
	SizeType i = 0;
	while(result && !done)
	{
		if(i >= bufferSize)
		{
			result = false;
			continue;
		}
		if(*(data + i) == terminator)
		{
			done = true;
			continue;
		}
		SizeType remainingCodeUnitsAvailable = 9999;
		UnicodeCodePoint codePoint;
		if(!ConvertWCharTToUnicodeCodePoint((data + i), codePoint, bufferSize, i))
		{
			result = false;
			continue;
		}
		result &= WriteCharInternalAsUTF32(codePoint, abyteOrder, remainingCodeUnitsAvailable, true);
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Internal fixed length text buffer write functions
//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from ASCII to UnicodeCodePoint
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint(*(memoryBuffer + i), codePoint);

		//Write the next character to the buffer
		result &= WriteCharInternalAsASCII(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from ASCII to UnicodeCodePoint
	UnicodeCodePoint codePoint;
	result &= ConvertCharToUnicodeCodePoint(paddingChar, codePoint);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsASCII(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsASCII(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from wchar_t to UnicodeCodePoint
		SizeType codeUnitsRead = 0;
		UnicodeCodePoint codePoint;
		result &= ConvertWCharTToUnicodeCodePoint((memoryBuffer + i), codePoint, (codeUnitsInMemory - i), codeUnitsRead);

		//Write the next character to the buffer
		result &= WriteCharInternalAsASCII(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from wchar_t to UnicodeCodePoint
	SizeType codeUnitsRead = 0;
	UnicodeCodePoint codePoint;
	result &= ConvertWCharTToUnicodeCodePoint(&paddingChar, codePoint, 1, codeUnitsRead);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsASCII(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from ASCII to UnicodeCodePoint
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint(*(memoryBuffer + i), codePoint);

		//Write the next character to the buffer
		result &= WriteCharInternalAsUTF8(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from ASCII to UnicodeCodePoint
	UnicodeCodePoint codePoint;
	result &= ConvertCharToUnicodeCodePoint(paddingChar, codePoint);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsUTF8(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsUTF8(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from wchar_t to UnicodeCodePoint
		SizeType codeUnitsRead = 0;
		UnicodeCodePoint codePoint;
		result &= ConvertWCharTToUnicodeCodePoint((memoryBuffer + i), codePoint, (codeUnitsInMemory - i), codeUnitsRead);

		//Write the next character to the buffer
		result &= WriteCharInternalAsUTF8(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from wchar_t to UnicodeCodePoint
	SizeType codeUnitsRead = 0;
	UnicodeCodePoint codePoint;
	result &= ConvertWCharTToUnicodeCodePoint(&paddingChar, codePoint, 1, codeUnitsRead);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsUTF8(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from ASCII to UnicodeCodePoint
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint(*(memoryBuffer + i), codePoint);

		//Write the next character to the buffer
		result &= WriteCharInternalAsUTF16(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from ASCII to UnicodeCodePoint
	UnicodeCodePoint codePoint;
	result &= ConvertCharToUnicodeCodePoint(paddingChar, codePoint);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsUTF16(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsUTF16(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from wchar_t to UnicodeCodePoint
		SizeType codeUnitsRead = 0;
		UnicodeCodePoint codePoint;
		result &= ConvertWCharTToUnicodeCodePoint((memoryBuffer + i), codePoint, (codeUnitsInMemory - i), codeUnitsRead);

		//Write the next character to the buffer
		result &= WriteCharInternalAsUTF16(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from wchar_t to UnicodeCodePoint
	SizeType codeUnitsRead = 0;
	UnicodeCodePoint codePoint;
	result &= ConvertWCharTToUnicodeCodePoint(&paddingChar, codePoint, 1, codeUnitsRead);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsUTF16(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const char* memoryBuffer, SizeType codeUnitsInMemory, char paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from ASCII to UnicodeCodePoint
		UnicodeCodePoint codePoint;
		result &= ConvertCharToUnicodeCodePoint(*(memoryBuffer + i), codePoint);

		//Write the next character to the buffer
		result &= WriteCharInternalAsUTF32(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from ASCII to UnicodeCodePoint
	UnicodeCodePoint codePoint;
	result &= ConvertCharToUnicodeCodePoint(paddingChar, codePoint);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsUTF32(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool Stream::WriteTextInternalFixedLengthBufferAsUTF32(ByteOrder abyteOrder, SizeType codeUnitsInStream, const wchar_t* memoryBuffer, SizeType codeUnitsInMemory, wchar_t paddingChar)
{
	bool result = true;

	//Write the source string to the target buffer
	bool foundEndOfString = false;
	for(SizeType i = 0; !foundEndOfString && (i < codeUnitsInMemory); ++i)
	{
		//Check if we've reached the end of the source string
		if(*(memoryBuffer + i) == '\0')
		{
			foundEndOfString = true;
			continue;
		}

		//Convert from wchar_t to UnicodeCodePoint
		SizeType codeUnitsRead = 0;
		UnicodeCodePoint codePoint;
		result &= ConvertWCharTToUnicodeCodePoint((memoryBuffer + i), codePoint, (codeUnitsInMemory - i), codeUnitsRead);

		//Write the next character to the buffer
		result &= WriteCharInternalAsUTF32(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	//Convert our padding char from wchar_t to UnicodeCodePoint
	SizeType codeUnitsRead = 0;
	UnicodeCodePoint codePoint;
	result &= ConvertWCharTToUnicodeCodePoint(&paddingChar, codePoint, 1, codeUnitsRead);

	//Pad the buffer to the target length
	while(result && (codeUnitsInStream > 0))
	{
		result &= WriteCharInternalAsUTF32(codePoint, abyteOrder, codeUnitsInStream, false);
	}

	return result;
}

//----------------------------------------------------------------------------------------
//Internal type-independent data write functions
//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternal(T data, ByteOrder abyteOrder)
{
	switch(abyteOrder)
	{
	case BYTEORDER_PLATFORM:
		return WriteBinaryNativeByteOrder(data);
	case BYTEORDER_BIGENDIAN:
		return WriteDataInternalBigEndian(data);
	case BYTEORDER_LITTLEENDIAN:
		return WriteDataInternalLittleEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternal(T data)
{
	switch(byteOrder)
	{
	case BYTEORDER_PLATFORM:
		return WriteBinaryNativeByteOrder(data);
	case BYTEORDER_BIGENDIAN:
		return WriteDataInternalBigEndian(data);
	case BYTEORDER_LITTLEENDIAN:
		return WriteDataInternalLittleEndian(data);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternalBigEndian(T data)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return WriteBinaryNativeByteOrder(data);
#else
	return WriteBinaryInvertedByteOrder(data);
#endif
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternalLittleEndian(T data)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return WriteBinaryInvertedByteOrder(data);
#else
	return WriteBinaryNativeByteOrder(data);
#endif
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternal(const T* data, SizeType length, ByteOrder abyteOrder)
{
	switch(abyteOrder)
	{
	case BYTEORDER_PLATFORM:
		return WriteBinaryNativeByteOrder(data, length);
	case BYTEORDER_BIGENDIAN:
		return WriteDataInternalBigEndian(data, length);
	case BYTEORDER_LITTLEENDIAN:
		return WriteDataInternalLittleEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternal(const T* data, SizeType length)
{
	switch(byteOrder)
	{
	case BYTEORDER_PLATFORM:
		return WriteBinaryNativeByteOrder(data, length);
	case BYTEORDER_BIGENDIAN:
		return WriteDataInternalBigEndian(data, length);
	case BYTEORDER_LITTLEENDIAN:
		return WriteDataInternalLittleEndian(data, length);
	}
	return false;
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternalBigEndian(const T* data, SizeType length)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return WriteBinaryNativeByteOrder(data, length);
#else
	return WriteBinaryInvertedByteOrder(data, length);
#endif
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteDataInternalLittleEndian(const T* data, SizeType length)
{
#if STREAM_PLATFORMBYTEORDER_BIGENDIAN
	return WriteBinaryInvertedByteOrder(data, length);
#else
	return WriteBinaryNativeByteOrder(data, length);
#endif
}

//----------------------------------------------------------------------------------------
//Inverted byte order write functions
//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteBinaryInvertedByteOrder(T data)
{
	//Invert the byte order
	T temp;
	unsigned char* binaryData = (unsigned char*)&data;
	unsigned char* binaryTemp = (unsigned char*)&temp;
	for(unsigned int i = 0; i < sizeof(data); ++i)
	{
		*(binaryTemp + i) = *(binaryData + ((sizeof(data) - 1) - i));
	}

	//Write the data in the native byte order
	return WriteBinaryNativeByteOrder(temp);
}

//----------------------------------------------------------------------------------------
template<class T> bool Stream::WriteBinaryInvertedByteOrder(const T* data, SizeType length)
{
	//Invert the byte order
	T* temp = new T[(size_t)length];

	unsigned int dataTypeSize = sizeof(*data);
	for(unsigned int entry = 0; entry < length; ++entry)
	{
		unsigned char* binaryData = (unsigned char*)&data[entry];
		unsigned char* binaryTemp = (unsigned char*)&temp[entry];
		for(unsigned int i = 0; i < dataTypeSize; ++i)
		{
			*(binaryTemp + i) = *(binaryData + ((dataTypeSize - 1) - i));
		}
	}

	//Write the data in the native byte order
	return WriteBinaryNativeByteOrder(temp, length);
}

//----------------------------------------------------------------------------------------
//Bool conversion functions
//----------------------------------------------------------------------------------------
unsigned char Stream::BoolToByte(bool data)
{
	return (data? 1: 0);
}

//----------------------------------------------------------------------------------------
bool Stream::ByteToBool(unsigned char data)
{
	return (data != 0);
}

} //Close namespace Stream
