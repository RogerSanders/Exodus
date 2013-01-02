#include "File.h"
#include <stdlib.h>
namespace Stream {

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
File::~File()
{
	Close();
	delete fileBuffer;
}

//----------------------------------------------------------------------------------------
//File position
//----------------------------------------------------------------------------------------
bool File::IsAtEnd() const
{
	return (GetStreamPos() >= Size());
}

//----------------------------------------------------------------------------------------
File::SizeType File::Size() const
{
	SizeType fileSize = 0;
	if(fileOpen)
	{
		LARGE_INTEGER fileSizeWindows;
		BOOL getFileSizeExReturn = GetFileSizeEx(fileHandle, &fileSizeWindows);
		if(getFileSizeExReturn != 0)
		{
			fileSize = (SizeType)fileSizeWindows.QuadPart;

			//Adjust the reported file size to take into account any unwritten data
			//currently held in the data buffer.
			if(bufferInWriteMode)
			{
				SizeType virtualFilePos = GetStreamPos();
				virtualFilePos += (SizeType)bufferPosOffset;
				if(virtualFilePos > fileSize)
				{
					fileSize = virtualFilePos;
				}
			}
		}
	}
	return fileSize;
}

//----------------------------------------------------------------------------------------
File::SizeType File::GetStreamPos() const
{
	SizeType streamPos = 0;
	if(fileOpen)
	{
		LARGE_INTEGER dummyFilePointer;
		dummyFilePointer.QuadPart = 0;
		LARGE_INTEGER currentFilePointer;
		BOOL setFilePointerExReturn = SetFilePointerEx(fileHandle, dummyFilePointer, &currentFilePointer, FILE_CURRENT);
		if(setFilePointerExReturn != 0)
		{
			streamPos = (SizeType)currentFilePointer.QuadPart;

			//Adjust the reported file position to take into account our position in the
			//data buffer.
			if(bufferInWriteMode)
			{
				streamPos += (SizeType)bufferPosOffset;
			}
			else
			{
				streamPos -= (SizeType)bytesRemainingInBuffer;
			}
		}
	}
	return streamPos;
}

//----------------------------------------------------------------------------------------
void File::SetStreamPos(SizeType position)
{
	if(fileOpen)
	{
		//Empty the contents of the data buffer before we perform a seek operation
		EmptyDataBuffer();

		//Set the new file seek position
		LARGE_INTEGER filePointer;
		filePointer.QuadPart = (LONGLONG)position;
		SetFilePointerEx(fileHandle, filePointer, NULL, FILE_BEGIN);
	}
}

//----------------------------------------------------------------------------------------
bool File::SkipBytes(SizeType byteCount)
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
bool File::ReadBinaryNativeByteOrder(char& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(signed char& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned char& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(wchar_t& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(short& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned short& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(int& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned int& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(long long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned long long& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(float& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(double& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(long double& data)
{
	return ReadBinary((void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
//Native byte order array read functions
//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(char* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(signed char* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned char* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(wchar_t* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(short* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned short* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(int* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned int* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(long* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned long* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(long long* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(unsigned long long* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(float* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(double* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::ReadBinaryNativeByteOrder(long double* data, SizeType length)
{
	return ReadBinary((void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
//Write functions
//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(char data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(signed char data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(unsigned char data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(wchar_t data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(short data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(unsigned short data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(int data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(unsigned int data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(unsigned long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(long long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(unsigned long long data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(float data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(double data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(long double data)
{
	return WriteBinary((const void*)&data, (unsigned int)sizeof(data));
}

//----------------------------------------------------------------------------------------
//Native byte order array write functions
//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const char* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const signed char* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const unsigned char* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const wchar_t* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const short* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const unsigned short* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const int* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const unsigned int* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const long* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const unsigned long* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const long long* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const unsigned long long* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const float* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const double* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
bool File::WriteBinaryNativeByteOrder(const long double* data, SizeType length)
{
	return WriteBinary((const void*)data, (unsigned int)length);
}

//----------------------------------------------------------------------------------------
//String conversion functions
//----------------------------------------------------------------------------------------
std::wstring File::ConvertStringToWString(const std::string& source)
{
	size_t stringLength = source.length();
	std::wstring target;
	if(stringLength > 0)
	{
		target.resize(stringLength);
		for(unsigned int i = 0; i < stringLength; ++i)
		{
			mbtowc(&target[i], &source[i], 1);
		}
	}
	return target;
}

} //Close namespace Stream
