#include "Buffer.h"
namespace Stream {

//----------------------------------------------------------------------------------------
//Buffer position
//----------------------------------------------------------------------------------------
bool Buffer::IsAtEnd() const
{
	return (streamPos >= bufferSize);
}

//----------------------------------------------------------------------------------------
Buffer::SizeType Buffer::Size() const
{
	return bufferSize;
}

//----------------------------------------------------------------------------------------
Buffer::SizeType Buffer::GetStreamPos() const
{
	return streamPos;
}

//----------------------------------------------------------------------------------------
void Buffer::SetStreamPos(SizeType position)
{
	streamPos = position;
}

//----------------------------------------------------------------------------------------
bool Buffer::SkipBytes(SizeType byteCount)
{
	//Return false if there are less than the requested number of bytes left in the buffer
	if((streamPos + byteCount) > bufferSize)
	{
		return false;
	}

	streamPos += byteCount;
	return true;
}

//----------------------------------------------------------------------------------------
//Native byte order read functions
//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(char& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(signed char& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned char& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(wchar_t& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(short& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned short& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(int& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned int& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(long& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned long& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(long long& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned long long& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(float& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(double& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(long double& data)
{
	return ReadBinary((unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
//Native byte order array read functions
//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(char* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(signed char* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned char* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(wchar_t* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(short* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned short* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(int* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned int* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(long* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned long* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(long long* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(unsigned long long* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(float* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(double* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::ReadBinaryNativeByteOrder(long double* data, SizeType length)
{
	return ReadBinary((unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
//Native byte order write functions
//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(char data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(signed char data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(unsigned char data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(wchar_t data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(short data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(unsigned short data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(int data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(unsigned int data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(long data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(unsigned long data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(long long data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(unsigned long long data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(float data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(double data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(long double data)
{
	return WriteBinary((const unsigned char*)&data, sizeof(data));
}

//----------------------------------------------------------------------------------------
//Native byte order array write functions
//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const char* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const signed char* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const unsigned char* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const wchar_t* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const short* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const unsigned short* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const int* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const unsigned int* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const long* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const unsigned long* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const long long* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const unsigned long long* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const float* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const double* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool Buffer::WriteBinaryNativeByteOrder(const long double* data, SizeType length)
{
	return WriteBinary((const unsigned char*)data, (length * sizeof(*data)));
}

} //Close namespace Stream
