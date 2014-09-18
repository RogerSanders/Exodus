#ifndef __MARSHALSUPPORT_MARSHALOUT_H__
#define __MARSHALSUPPORT_MARSHALOUT_H__
#include "InteropPreprocessorMacros.h"
#include "STLObjectTarget.h"
namespace InteropSupport {
namespace Marshal {

template <class T>
class Out
{
public:
	//Constructors
	Out<T>(T& target)
	:_target(target), _targetReference(_target)
	{ }

	//Marshal functions
	void Set(const T& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
	}
	Out& operator=(const T& sourceObject)
	{
		//Note that we use a reference to the marshaller object here. This is done to
		//ensure that the compiler can't optimize away any virtual function calls we're
		//using to enforce code boundaries between assemblies. If the compiler could be
		//sure of the actual type of the target marshaller object, it could attempt to do
		//this. Since this class is designed to be passed by reference across assembly
		//boundaries, the compiler can't assume anything about the existing contents of
		//the object when it is passed as an argument to a method in the target assembly,
		//and therefore cannot optimize away our virtual function boundaries.
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}

private:
	//Disable copying and moving
	Out(const Out& source);
	Out& operator=(const Out& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	Out(Out&& source);
	Out& operator=(Out&& source);
#endif

private:
	STLObjectTarget<T> _target;
	const ISTLObjectTarget<T>& _targetReference;
};

} //Close namespace Marshal
} //Close namespace InteropSupport
#endif
