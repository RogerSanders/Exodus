#ifndef __MARSHALSUPPORT_MARSHALOUT_H__
#define __MARSHALSUPPORT_MARSHALOUT_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalTarget.h"
#include "MarshalInOut.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#endif
namespace MarshalSupport {
namespace Marshal {

template <class T>
class Out
{
public:
	//Constructors
	inline Out<T>(T& target)
	:_targetReference(_target), _target(target)
	{ }
	inline Out(const InOut<T>& target)
	:_targetReference(target._targetReference), _target(*((T*)0))
	{ }

	//Marshal functions
	inline void Set(const T& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const Out& operator=(const T& sourceObject) const
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
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void Set(T&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::forward<T>(sourceObject));
	}
	inline const Out& operator=(T&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::forward<T>(sourceObject));
		return *this;
	}
#endif

private:
	//Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of
	//the MarshalSource and MarshalTarget classes is permitted to vary on either side of
	//an assembly boundary, so we need to ensure our reference to the interface for this
	//object can be reliably retrieved, even if the size of the target object is different
	//to what is expected.
	const IMarshalTarget<T>& _targetReference;
	MarshalTarget<T> _target;
};

} //Close namespace Marshal
} //Close namespace MarshalSupport
#endif
