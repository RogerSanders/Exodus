#ifndef __VIEWBINARY_H__
#define __VIEWBINARY_H__
namespace Stream {

class ViewBinary
{
public:
	//Constructors
	inline ViewBinary(IStreamNonSeekable& astream);

	//Binary stream functions
	template<class T> ViewBinary& operator>>(T& data);
	template<class T> ViewBinary& operator<<(const T& data);
	template<class T> ViewBinary& operator>>(std::vector<T>& data);
	template<class T> ViewBinary& operator<<(const std::vector<T>& data);

private:
	IStreamNonSeekable& stream;
};

} //Close namespace Stream
#include "ViewBinary.inl"
#endif
