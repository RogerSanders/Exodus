#ifndef __NOVETYPENOCOPY_H__
#define __NOVETYPENOCOPY_H__
#include "MarshalSupport/MarshalSupport.pkg"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
using namespace MarshalSupport::Operators;
#else
using namespace MarshalSupport;
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
class MoveTypeNoCopy
{
public:
	MoveTypeNoCopy()
	:someData(-21346)
	{ }
	explicit MoveTypeNoCopy(int value)
	:someData(value)
	{ }
	MoveTypeNoCopy(MoveTypeNoCopy&& source)
	:someData(source.someData)
	{ }
	MoveTypeNoCopy& operator=(MoveTypeNoCopy&& source)
	{
		someData = source.someData;
		return *this;
	}

	int GetValue()
	{
		return someData;
	}

	bool operator==(const MoveTypeNoCopy& target) const
	{
		return (someData == target.someData);
	}

private:
	MoveTypeNoCopy(const MoveTypeNoCopy&) MARSHALSUPPORT_DELETEMETHOD;
	MoveTypeNoCopy& operator=(const MoveTypeNoCopy& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	int someData;
};
#endif

#endif
