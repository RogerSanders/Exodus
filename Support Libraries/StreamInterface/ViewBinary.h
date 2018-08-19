#ifndef __VIEWBINARY_H__
#define __VIEWBINARY_H__
#include "IStreamNonSeekable.h"
#include "IStream.h"
#include <vector>
namespace Stream {

class ViewBinary
{
public:
	// Constructors
	inline ViewBinary(IStreamNonSeekable& stream);

	// State functions
	inline bool NoErrorsOccurred() const;
	inline void ClearErrorState();
	inline bool IsAtEnd() const;
	inline IStream::ByteOrder GetViewByteOrder() const;
	inline void SetViewByteOrder(IStream::ByteOrder byteOrder);

	// Binary stream functions
	template<class T> ViewBinary& operator>>(T& data);
	template<class T> ViewBinary& operator<<(const T& data);
	template<class T> ViewBinary& operator>>(std::vector<T>& data);
	inline ViewBinary& operator>>(std::vector<bool>& data);
	inline ViewBinary& operator>>(std::vector<char>& data);
	inline ViewBinary& operator>>(std::vector<signed char>& data);
	inline ViewBinary& operator>>(std::vector<unsigned char>& data);
	inline ViewBinary& operator>>(std::vector<wchar_t>& data);
	inline ViewBinary& operator>>(std::vector<short>& data);
	inline ViewBinary& operator>>(std::vector<unsigned short>& data);
	inline ViewBinary& operator>>(std::vector<int>& data);
	inline ViewBinary& operator>>(std::vector<unsigned int>& data);
	inline ViewBinary& operator>>(std::vector<long>& data);
	inline ViewBinary& operator>>(std::vector<unsigned long>& data);
	inline ViewBinary& operator>>(std::vector<long long>& data);
	inline ViewBinary& operator>>(std::vector<unsigned long long>& data);
	inline ViewBinary& operator>>(std::vector<float>& data);
	inline ViewBinary& operator>>(std::vector<double>& data);
	inline ViewBinary& operator>>(std::vector<long double>& data);
	template<class T> ViewBinary& operator<<(const std::vector<T>& data);
	inline ViewBinary& operator<<(const std::vector<bool>& data);
	inline ViewBinary& operator<<(const std::vector<char>& data);
	inline ViewBinary& operator<<(const std::vector<signed char>& data);
	inline ViewBinary& operator<<(const std::vector<unsigned char>& data);
	inline ViewBinary& operator<<(const std::vector<wchar_t>& data);
	inline ViewBinary& operator<<(const std::vector<short>& data);
	inline ViewBinary& operator<<(const std::vector<unsigned short>& data);
	inline ViewBinary& operator<<(const std::vector<int>& data);
	inline ViewBinary& operator<<(const std::vector<unsigned int>& data);
	inline ViewBinary& operator<<(const std::vector<long>& data);
	inline ViewBinary& operator<<(const std::vector<unsigned long>& data);
	inline ViewBinary& operator<<(const std::vector<long long>& data);
	inline ViewBinary& operator<<(const std::vector<unsigned long long>& data);
	inline ViewBinary& operator<<(const std::vector<float>& data);
	inline ViewBinary& operator<<(const std::vector<double>& data);
	inline ViewBinary& operator<<(const std::vector<long double>& data);

private:
	IStreamNonSeekable& _stream;
	IStream::ByteOrder _byteOrder;
	bool _noErrorState;
};

} // Close namespace Stream
#include "ViewBinary.inl"
#endif
