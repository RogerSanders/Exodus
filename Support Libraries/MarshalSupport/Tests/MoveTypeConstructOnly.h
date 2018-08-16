#ifndef __NOVETYPECONSTRUCTONLY_H__
#define __NOVETYPECONSTRUCTONLY_H__
#include "MarshalSupport/MarshalSupport.pkg"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
using namespace MarshalSupport::Operators;
#else
using namespace MarshalSupport;
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
class MoveTypeConstructOnly
{
public:
	MoveTypeConstructOnly()
	:someData(-21346)
	{ }
	explicit MoveTypeConstructOnly(int value)
	:someData(value)
	{ }
	MoveTypeConstructOnly(MoveTypeConstructOnly&& source)
	:someData(source.someData)
	{ }

	int GetValue()
	{
		return someData;
	}

	bool operator==(const MoveTypeConstructOnly& target) const
	{
		return (someData == target.someData);
	}

private:
	MoveTypeConstructOnly(const MoveTypeConstructOnly&) MARSHALSUPPORT_DELETEMETHOD;
	MoveTypeConstructOnly& operator=(const MoveTypeConstructOnly& source) MARSHALSUPPORT_DELETEMETHOD;
	MoveTypeConstructOnly& operator=(MoveTypeConstructOnly&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	int someData;
};
#endif

#endif
