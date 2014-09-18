#ifndef __MARSHALSUPPORT_MARSHALINOUT_H__
#define __MARSHALSUPPORT_MARSHALINOUT_H__
#include "InteropPreprocessorMacros.h"
#include "STLObjectSource.h"
#include "STLObjectTarget.h"
namespace InteropSupport {
namespace Marshal {

template <class T>
class InOut
{
public:
	//Constructors
	InOut(T& target)
	:_source(target), _sourceReference(_source), _target(target), _targetReference(_target)
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
	void Set(const T& sourceObject) const
	{
		//See notes on Get method
		_targetReference.MarshalFrom(sourceObject);
	}
	Out& operator=(const T& sourceObject)
	{
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}

	//Implicit conversion
	operator T() const
	{
		return Get();
	}

private:
	//Disable copying and moving
	InOut(const InOut& sourceObject);
	InOut& operator=(const InOut& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	InOut(InOut&& sourceObject);
	InOut& operator=(InOut&& source);
#endif

private:
	STLObjectSource<T> _source;
	const ISTLObjectSource<T>& _sourceReference;
	STLObjectTarget<T> _target;
	const ISTLObjectTarget<T>& _targetReference;
};

} //Close namespace Marshal
} //Close namespace InteropSupport
#endif
