#ifndef __MARSHALSUPPORT_MARSHALIN_H__
#define __MARSHALSUPPORT_MARSHALIN_H__
#include "InteropPreprocessorMacros.h"
#include "STLObjectSource.h"
namespace InteropSupport {
namespace Marshal {

template <class T>
class In
{
public:
	//Constructors
	In(const T& source)
	:_source(source), _sourceReference(_source)
	{ }

	//Marshal functions
	T Get() const
	{
		T targetObject;
		Get(targetObject);
		return targetObject;
	}
	void Get(T& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to
		//ensure that the compiler can't optimize away any virtual function calls we're
		//using to enforce code boundaries between assemblies. If the compiler could be
		//sure of the actual type of the target marshaller object, it could attempt to do
		//this. Since this class is designed to be passed by reference across assembly
		//boundaries, the compiler can't assume anything about the existing contents of
		//the object when it is passed as an argument to a method in the target assembly,
		//and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalTo(targetObject);
	}

	//Implicit conversion
	operator T() const
	{
		return Get();
	}

private:
	//Disable copying and moving
	In(const In& source);
	In& operator=(const In& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	In(In&& source);
	In& operator=(In&& source);
#endif

private:
	STLObjectSource<T> _source;
	const ISTLObjectSource<T>& _sourceReference;
};

} //Close namespace Marshal
} //Close namespace InteropSupport
#endif
