#ifndef __VIEWBINARY_H__
#define __VIEWBINARY_H__
namespace Stream {

class ViewBinary
{
public:
	//Constructors
	inline ViewBinary(IStream& astream);

	//Binary stream functions
	template<class T> ViewBinary& operator>>(T& data);
	template<class T> ViewBinary& operator<<(const T& data);

private:
	IStream& stream;
};

} //Close namespace Stream
#include "ViewBinary.inl"
#endif
