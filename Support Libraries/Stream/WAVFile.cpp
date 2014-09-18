#include "WAVFile.h"
#include <stdlib.h>
namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WAVFile::~WAVFile()
{
	Close();
	delete[] fileBuffer;
}

//----------------------------------------------------------------------------------------
//File position
//----------------------------------------------------------------------------------------
bool WAVFile::IsAtEnd() const
{
	return true;
}

//----------------------------------------------------------------------------------------
bool WAVFile::SkipBytes(SizeType byteCount)
{
	bool result = false;
	if(fileOpen)
	{
		result = true;
		unsigned char temp;
		for(SizeType i = 0; i < byteCount; ++i)
		{
			result &= ReadData(temp);
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Read functions
//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(char& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(signed char& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned char& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(wchar_t& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(short& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned short& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(int& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned int& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(long long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned long long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(float& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(double& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(long double& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
//Native byte order array read functions
//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(char* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(signed char* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned char* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(wchar_t* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(short* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned short* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(int* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned int* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(long* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned long* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(long long* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(unsigned long long* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(float* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(double* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::ReadBinaryNativeByteOrder(long double* data, SizeType length)
{
	return ReadBinary((void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
//Write functions
//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(char data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(signed char data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(unsigned char data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(wchar_t data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(short data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(unsigned short data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(int data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(unsigned int data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(unsigned long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(long long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(unsigned long long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(float data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(double data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(long double data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
//Native byte order array write functions
//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const char* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const signed char* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const unsigned char* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const wchar_t* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const short* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const unsigned short* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const int* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const unsigned int* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const long* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const unsigned long* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const long long* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const unsigned long long* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const float* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const double* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

//----------------------------------------------------------------------------------------
bool WAVFile::WriteBinaryNativeByteOrder(const long double* data, SizeType length)
{
	return WriteBinary((const void*)data, (length * sizeof(*data)));
}

} //Close namespace Stream
