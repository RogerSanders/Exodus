#include "Debug/Debug.pkg"
namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Buffer::Buffer(SizeType size)
:_streamPos(0), _bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(SizeType size, SizeType sizeIncrement)
:_streamPos(0), _bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding textEncoding, SizeType size)
:Stream(textEncoding), _streamPos(0), _bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding textEncoding, SizeType size, SizeType sizeIncrement)
:Stream(textEncoding), _streamPos(0), _bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding textEncoding, NewLineEncoding newLineEncoding, SizeType size)
:Stream(textEncoding, newLineEncoding), _streamPos(0), _bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding textEncoding, NewLineEncoding newLineEncoding, SizeType size, SizeType sizeIncrement)
:Stream(textEncoding, newLineEncoding), _streamPos(0), _bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding textEncoding, NewLineEncoding newLineEncoding, ByteOrder byteOrder, SizeType size)
:Stream(textEncoding, newLineEncoding, byteOrder), _streamPos(0), _bufferSizeIncrement(1024)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
Buffer::Buffer(TextEncoding textEncoding, NewLineEncoding newLineEncoding, ByteOrder byteOrder, SizeType size, SizeType sizeIncrement)
:Stream(textEncoding, newLineEncoding, byteOrder), _streamPos(0), _bufferSizeIncrement(sizeIncrement)
{
	Resize(size);
}

//----------------------------------------------------------------------------------------
//Buffer management
//----------------------------------------------------------------------------------------
void Buffer::Resize(SizeType size)
{
	_bufferSize = size;
	if (_bufferSize > (SizeType)_buffer.size())
	{
		_buffer.resize((size_t)((_bufferSize - (_bufferSize % _bufferSizeIncrement)) + _bufferSizeIncrement));
	}
}

//----------------------------------------------------------------------------------------
void Buffer::CheckBufferSize(SizeType writeSize)
{
	if ((_streamPos + writeSize) > _bufferSize)
	{
		Resize(_streamPos + writeSize);
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
	if (_buffer.empty())
	{
		return (unsigned char*)0;
	}
	else
	{
		return (unsigned char*)&_buffer[0];
	}
}

//----------------------------------------------------------------------------------------
//Dereference operators
//----------------------------------------------------------------------------------------
const unsigned char& Buffer::operator[](SizeType position) const
{
	DebugAssert(position < _bufferSize);
	return _buffer[(size_t)position];
}

//----------------------------------------------------------------------------------------
unsigned char& Buffer::operator[](SizeType position)
{
	DebugAssert(position < _bufferSize);
	return _buffer[(size_t)position];
}

//----------------------------------------------------------------------------------------
//Internal read/write functions
//----------------------------------------------------------------------------------------
bool Buffer::ReadBinary(unsigned char* rawData, SizeType bytesToRead)
{
	//Return false if a read tries to pass the end of the buffer
	if ((_streamPos + bytesToRead) > _bufferSize)
	{
		return false;
	}

	//Read the data from the buffer
	for (SizeType i = 0; i < bytesToRead; ++i)
	{
		*(rawData + i) = _buffer[(size_t)_streamPos++];
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinary(const unsigned char* rawData, SizeType bytesToWrite)
{
	//Resize the buffer if a write tries to pass the end of the buffer
	if ((_streamPos + bytesToWrite) > _bufferSize)
	{
		Resize(_streamPos + bytesToWrite);
	}

	//Write the data to the buffer
	for (SizeType i = 0; i < bytesToWrite; ++i)
	{
		_buffer[(size_t)_streamPos++] = *(rawData + i);
	}
	return true;
}

} //Close namespace Stream
