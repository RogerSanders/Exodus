#ifndef __VIEWTEXT_H__
#define __VIEWTEXT_H__
#include <sstream>
#include "IStream.h"
namespace Stream {

class ViewText
{
public:
	// Constructors
	inline ViewText(IStream& stream);

	// State functions
	inline bool NoErrorsOccurred() const;
	inline void ClearErrorState();
	inline bool IsAtEnd() const;
	inline IStream::ByteOrder GetViewByteOrder() const;
	inline void SetViewByteOrder(IStream::ByteOrder byteOrder);

	// Text-based access functions
	template<class T> bool Read(T& data);
	template<class T> bool Write(const T& data);
	template<size_t S> bool Write(const char(&data)[S]);
	template<size_t S> bool Write(const wchar_t(&data)[S]);

	// Text string read functions
	inline bool ReadTextString(std::string& data, bool stopAtNewline = true);
	inline bool ReadTextString(std::wstring& data, bool stopAtNewline = true);

	// Text stream functions
	template<class T> ViewText& operator>>(T& data);
	template<class T> ViewText& operator<<(const T& data);

private:
	// String length functions
	inline static IStream::SizeType GetStringLength(const char* data);
	inline static IStream::SizeType GetStringLength(const wchar_t* data);
	inline static IStream::SizeType GetStringLength(const char* data, size_t bufferSize);
	inline static IStream::SizeType GetStringLength(const wchar_t* data, size_t bufferSize);

private:
	IStream& _stream;
	std::wstringstream _wcharStream;
	IStream::ByteOrder _byteOrder;
	bool _noErrorState;
};

} // Close namespace Stream
#include "ViewText.inl"
#endif
