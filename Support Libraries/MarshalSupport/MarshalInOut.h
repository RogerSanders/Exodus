#ifndef __MARSHALSUPPORT_MARSHALINOUT_H__
#define __MARSHALSUPPORT_MARSHALINOUT_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalSource.h"
#include "MarshalTarget.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#endif
namespace MarshalSupport {
namespace Marshal {

template <class T>
class InOut
{
public:
	//Friend declarations
	template <class T>
	friend class In;
	template <class T>
	friend class Out;

public:
	//Constructors
	inline InOut(T& target)
	:_sourceReference(_source), _targetReference(_target), _source(target), _target(target)
	{ }

	//Marshal functions
	inline T Get() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void Get(T& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to
		//ensure that the compiler can't optimize away any virtual function calls we're
		//using to enforce code boundaries between assemblies. If the compiler could be
		//sure of the actual type of the target marshaller object, it could attempt to do
		//this. Since this class is designed to be passed by reference across assembly
		//boundaries, the compiler can't assume anything about the existing contents of
		//the object when it is passed as an argument to a method in the target assembly,
		//and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalToWithoutMove(targetObject);
	}
	inline void Set(const T& sourceObject) const
	{
		//See notes on Get method
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const InOut& operator=(const T& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void Set(T&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::forward<T>(sourceObject));
	}
	inline const InOut& operator=(T&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::forward<T>(sourceObject));
		return *this;
	}
#endif

	//Implicit conversion
	inline operator T() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever
	//be. The only purpose of this constructor is to introduce a path by which the
	//_sourceReference and _targetReference members could be initialized with a value of
	//anything other than the addresses of the _source and _target members. Since this
	//type could be derived from elsewhere, and this constructor could be called by a
	//derived type, and we explicitly set the value of _sourceReference and
	//_targetReference here to an externally provided value, the optimizer cannot assume
	//what the contents of these members could be based on the other provided
	//constructors, making it impossible to optimize away virtual function calls within
	//the referenced object.
	inline InOut(bool, T& target, const T& source)
	:_sourceReference(source), _targetReference(target), _source(source), _target(target)
	{ }

private:
	//Disable copying and moving
	InOut(const InOut& sourceObject) MARSHALSUPPORT_DELETEMETHOD;
	InOut& operator=(const InOut& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	InOut(InOut&& sourceObject) MARSHALSUPPORT_DELETEMETHOD;
	InOut& operator=(InOut&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of
	//the MarshalSource and MarshalTarget classes is permitted to vary on either side of
	//an assembly boundary, so we need to ensure our reference to the interface for this
	//object can be reliably retrieved, even if the size of the target object is different
	//to what is expected.
	const IMarshalSource<T>& _sourceReference;
	const IMarshalTarget<T>& _targetReference;
	MarshalSource<T> _source;
	MarshalTarget<T> _target;
};

} //Close namespace Marshal
} //Close namespace MarshalSupport
#include "MarshalInOutStringOperators.inl"
#endif
