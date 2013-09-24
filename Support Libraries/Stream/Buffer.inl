#include "Debug/Debug.pkg"
namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Buffer::Buffer(SizeType size)
:streamPos(0), bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(SizeType size, SizeType sizeIncrement)
:streamPos(0), bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding atextEncoding, SizeType size)
:Stream(atextEncoding), streamPos(0), bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding atextEncoding, SizeType size, SizeType sizeIncrement)
:Stream(atextEncoding), streamPos(0), bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, SizeType size)
:Stream(atextEncoding, anewLineEncoding), streamPos(0), bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, SizeType size, SizeType sizeIncrement)
:Stream(atextEncoding, anewLineEncoding), streamPos(0), bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder, SizeType size)
:Stream(atextEncoding, anewLineEncoding, abyteOrder), streamPos(0), bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder, SizeType size, SizeType sizeIncrement)
:Stream(atextEncoding, anewLineEncoding, abyteOrder), streamPos(0), bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
//Buffer management
//----------------------------------------------------------------------------------------
void Buffer::Resize(SizeType size)
{
	bufferSize = size;
	if(bufferSize > (SizeType)buffer.size())
	{
		buffer.resize((size_t)((bufferSize - (bufferSize % bufferSizeIncrement)) + bufferSizeIncrement));
	}
}

//----------------------------------------------------------------------------------------
void Buffer::CheckBufferSize(SizeType writeSize)
{
	if((streamPos + writeSize) > bufferSize)
	{
		Resize(streamPos + writeSize);
	}
}

//----------------------------------------------------------------------------------------
unsigned char* Buffer::GetRawBuffer() const
{
	//We return a null pointer in the case that the internal buffer is empty, otherwise
	//the std::vector throws an exception. We've found its useful for the user to be able
	//to get the raw pointer for the internal buffer even when the buffer is empty, eg
	//when iterating through the buffer for elements while not at the end of the buffer.
	//In these cases the null pointer is never used, but its neater to obtain it before
	//checking the size of the buffer.
	if(buffer.empty())
	{
		return (unsigned char*)0;
	}
	else
	{
		return (unsigned char*)&buffer[0];
	}
}

//----------------------------------------------------------------------------------------
//Dereference operators
//----------------------------------------------------------------------------------------
const unsigned char& Buffer::operator[](SizeType position) const
{
	ReleaseAssert(position < bufferSize);
	return buffer[(size_t)position];
}

//----------------------------------------------------------------------------------------
unsigned char& Buffer::operator[](SizeType position)
{
	ReleaseAssert(position < bufferSize);
	return buffer[(size_t)position];
}

//----------------------------------------------------------------------------------------
//Internal read/write functions
//----------------------------------------------------------------------------------------
bool Buffer::ReadBinary(unsigned char* rawData, SizeType bytesToRead)
{
	//Return false if a read tries to pass the end of the buffer
	if((streamPos + bytesToRead) > bufferSize)
	{
		return false;
	}

	//Read the data from the buffer
	for(SizeType i = 0; i < bytesToRead; ++i)
	{
		*(rawData + i) = buffer[(size_t)streamPos++];
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinary(const unsigned char* rawData, SizeType bytesToWrite)
{
	//Resize the buffer if a write tries to pass the end of the buffer
	if((streamPos + bytesToWrite) > bufferSize)
	{
		Resize(streamPos + bytesToWrite);
	}

	//Write the data to the buffer
	for(SizeType i = 0; i < bytesToWrite; ++i)
	{
		buffer[(size_t)streamPos++] = *(rawData + i);
	}
	return true;
}

} //Close namespace Stream
