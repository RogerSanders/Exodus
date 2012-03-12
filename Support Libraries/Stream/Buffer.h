#include "Stream.h"
#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <vector>
namespace Stream {

class Buffer :public Stream
{
public:
	//Constructors
	inline explicit Buffer(SizeType size = 0);
	inline Buffer(SizeType size, SizeType sizeIncrement);
	inline Buffer(TextEncoding atextEncoding, SizeType size);
	inline Buffer(TextEncoding atextEncoding, SizeType size, SizeType sizeIncrement);
	inline Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, SizeType size);
	inline Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, SizeType size, SizeType sizeIncrement);
	inline Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder, SizeType size);
	inline Buffer(TextEncoding atextEncoding, NewLineEncoding anewLineEncoding, ByteOrder abyteOrder, SizeType size, SizeType sizeIncrement);

	//Buffer position
	virtual bool IsAtEnd() const;
	virtual SizeType Size() const;
	virtual SizeType GetStreamPos() const;
	virtual void SetStreamPos(SizeType position);
	virtual bool SkipBytes(SizeType byteCount);

	//Buffer management
	inline void Resize(SizeType size);
	inline unsigned char* GetRawBuffer() const;

	//Dereference operators
	inline const unsigned char& operator[](SizeType position) const;
	inline unsigned char& operator[](SizeType position);

protected:
	using Stream::ReadBinaryNativeByteOrder;
	using Stream::WriteBinaryNativeByteOrder;

	//Native byte order read functions
	virtual bool ReadBinaryNativeByteOrder(char& data);
	virtual bool ReadBinaryNativeByteOrder(signed char& data);
	virtual bool ReadBinaryNativeByteOrder(unsigned char& data);
	virtual bool ReadBinaryNativeByteOrder(wchar_t& data);
	virtual bool ReadBinaryNativeByteOrder(short& data);
	virtual bool ReadBinaryNativeByteOrder(unsigned short& data);
	virtual bool ReadBinaryNativeByteOrder(int& data);
	virtual bool ReadBinaryNativeByteOrder(unsigned int& data);
	virtual bool ReadBinaryNativeByteOrder(long& data);
	virtual bool ReadBinaryNativeByteOrder(unsigned long& data);
	virtual bool ReadBinaryNativeByteOrder(long long& data);
	virtual bool ReadBinaryNativeByteOrder(unsigned long long& data);
	virtual bool ReadBinaryNativeByteOrder(float& data);
	virtual bool ReadBinaryNativeByteOrder(double& data);
	virtual bool ReadBinaryNativeByteOrder(long double& data);

	//Native byte order array read functions
	virtual bool ReadBinaryNativeByteOrder(char* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(signed char* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(unsigned char* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(wchar_t* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(short* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(unsigned short* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(int* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(unsigned int* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(long* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(unsigned long* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(long long* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(unsigned long long* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(float* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(double* data, SizeType length);
	virtual bool ReadBinaryNativeByteOrder(long double* data, SizeType length);

	//Native byte order write functions
	virtual bool WriteBinaryNativeByteOrder(char data);
	virtual bool WriteBinaryNativeByteOrder(signed char data);
	virtual bool WriteBinaryNativeByteOrder(unsigned char data);
	virtual bool WriteBinaryNativeByteOrder(wchar_t data);
	virtual bool WriteBinaryNativeByteOrder(short data);
	virtual bool WriteBinaryNativeByteOrder(unsigned short data);
	virtual bool WriteBinaryNativeByteOrder(int data);
	virtual bool WriteBinaryNativeByteOrder(unsigned int data);
	virtual bool WriteBinaryNativeByteOrder(long data);
	virtual bool WriteBinaryNativeByteOrder(unsigned long data);
	virtual bool WriteBinaryNativeByteOrder(long long data);
	virtual bool WriteBinaryNativeByteOrder(unsigned long long data);
	virtual bool WriteBinaryNativeByteOrder(float data);
	virtual bool WriteBinaryNativeByteOrder(double data);
	virtual bool WriteBinaryNativeByteOrder(long double data);

	//Native byte order array write functions
	virtual bool WriteBinaryNativeByteOrder(const char* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const signed char* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const unsigned char* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const wchar_t* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const short* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const unsigned short* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const int* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const unsigned int* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const long* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const unsigned long* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const long long* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const unsigned long long* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const float* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const double* data, SizeType length);
	virtual bool WriteBinaryNativeByteOrder(const long double* data, SizeType length);

private:
	//Buffer management
	inline void CheckBufferSize(SizeType writeSize);

	//Internal read/write functions
	inline bool ReadBinary(unsigned char* rawData, unsigned int bytesToRead);
	inline bool WriteBinary(const unsigned char* rawData, unsigned int bytesToWrite);

private:
	std::vector<unsigned char> buffer;
	SizeType bufferSize;
	SizeType bufferSizeIncrement;
	SizeType streamPos;
};

} //Close namespace Stream
#include "Buffer.inl"
#endif
