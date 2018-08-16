#ifndef __SOMEINTERFACE_H__
#define __SOMEINTERFACE_H__
#include "ISomeInterface.h"

class SomeInterface : public ISomeInterface
{
public:
	virtual int SomeMethod() const
	{
		return 42;
	}

protected:
	virtual void Destroy()
	{
		::operator delete(this);
	}
};

#endif
