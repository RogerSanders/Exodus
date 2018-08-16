#ifndef __ISOMEINTERFACE_H__
#define __ISOMEINTERFACE_H__
#include "MarshalSupport/MarshalSupport.pkg"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
using namespace MarshalSupport::Operators;
#else
using namespace MarshalSupport;
#endif

class ISomeInterface
{
public:
	virtual int SomeMethod() const = 0;

public:
	static void operator delete(void* target)
	{
		if (target != 0)
		{
			static_cast<ISomeInterface*>(target)->Destroy();
		}
	}

protected:
	virtual void Destroy() = 0;
};

#endif
