#ifndef __VIEWTEXT_H__
#define __VIEWTEXT_H__
#include <sstream>
namespace Stream {

class ViewText
{
public:
	//Constructors
	inline ViewText(IStream& astream);

	//Text-based access functions
	template<class T> bool Read(T& data);
	template<> bool Read(bool& data);
	template<> bool Read(std::string& data);
	template<> bool Read(std::wstring& data);
	template<> bool Read(char& data);
	template<> bool Read(wchar_t& data);
	template<> bool Read(signed char& data);
	template<> bool Read(unsigned char& data);
	template<class T> bool Write(const T& data);
	template<> bool Write(const bool& data);
	template<> bool Write(const float& data);
	template<> bool Write(const double& data);
	template<> bool Write(const long double& data);
	template<> bool Write(const std::string& data);
	template<> bool Write(const std::wstring& data);
	template<> bool Write(char* const& data);
	template<> bool Write(wchar_t* const& data);
	template<> bool Write(const char* const& data);
	template<> bool Write(const wchar_t* const& data);
	template<size_t S> bool Write(const char(&data)[S]);
	template<size_t S> bool Write(const wchar_t(&data)[S]);
	template<> bool Write(const char& data);
	template<> bool Write(const wchar_t& data);
	template<> bool Write(const signed char& data);
	template<> bool Write(const unsigned char& data);

	//Text string read functions
	inline bool ReadTextString(std::string& data, bool stopAtNewline = true);
	inline bool ReadTextString(std::wstring& data, bool stopAtNewline = true);

	//Text stream functions
	template<class T> ViewText& operator>>(T& data);
	template<class T> ViewText& operator<<(const T& data);

private:
	//String length functions
	inline static IStream::SizeType GetStringLength(const char* data);
	inline static IStream::SizeType GetStringLength(const wchar_t* data);

private:
	IStream& stream;
	std::wstringstream wcharStream;
};

} //Close namespace Stream
#include "ViewTextManipulators.h"
#include "ViewText.inl"
#endif
