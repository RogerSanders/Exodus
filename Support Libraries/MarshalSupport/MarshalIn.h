#ifndef __MARSHALSUPPORT_MARSHALIN_H__
#define __MARSHALSUPPORT_MARSHALIN_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalSource.h"
#include "MarshalInOut.h"
#include "MarshalRet.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#endif
namespace MarshalSupport {
namespace Marshal {

//----------------------------------------------------------------------------------------
template <class T>
class In
{
public:
	//Constructors
	inline In(const T& source)
	:_sourceReference(_source), _source(source)
	{ }
	inline In(const InOut<T>& source)
	:_sourceReference(source._sourceReference), _source(*((T*)0))
	{ }
	inline In(const Ret<T>& source)
	:_sourceReference(source._data->GetMarshaller()), _source(*((T*)0))
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline In(T&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }
#endif

	//Marshal functions
	inline T Get() const
	{
		return _sourceReference.MarshalTo();
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
		_sourceReference.MarshalTo(targetObject);
	}
	inline T GetWithoutMove() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void GetWithoutMove(T& targetObject) const
	{
		_sourceReference.MarshalToWithoutMove(targetObject);
	}

	//Implicit conversion
	inline operator T() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of
	//the MarshalSource and MarshalTarget classes is permitted to vary on either side of
	//an assembly boundary, so we need to ensure our reference to the interface for this
	//object can be reliably retrieved, even if the size of the target object is different
	//to what is expected.
	const IMarshalSource<T>& _sourceReference;
	MarshalSource<T> _source;
};

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class In<std::basic_string<ElementType, traits, Alloc>>
{
public:
	//Constructors
	inline In(const std::basic_string<ElementType, traits, Alloc>& source)
	:_sourceReference(_source), _source(source)
	{ }
	inline In(const InOut<std::basic_string<ElementType, traits, Alloc>>& source)
	:_sourceReference(source._sourceReference), _source(*((std::basic_string<ElementType, traits, Alloc>*)0))
	{ }
	inline In(const Ret<std::basic_string<ElementType, traits, Alloc>>& source)
	:_sourceReference(source._data->GetMarshaller()), _source(*((std::basic_string<ElementType, traits, Alloc>*)0))
	{ }
	inline In(const ElementType* source)
	:_sourceReference(_source), _source(source)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline In(std::basic_string<ElementType, traits, Alloc>&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }
#endif

	//Marshal functions
	inline std::basic_string<ElementType, traits, Alloc> Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline void Get(std::basic_string<ElementType, traits, Alloc>& targetObject) const
	{
		_sourceReference.MarshalTo(targetObject);
	}
	inline std::basic_string<ElementType, traits, Alloc> GetWithoutMove() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void GetWithoutMove(std::basic_string<ElementType, traits, Alloc>& targetObject) const
	{
		_sourceReference.MarshalToWithoutMove(targetObject);
	}

	//Implicit conversion
	inline operator std::basic_string<ElementType, traits, Alloc>() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of
	//the MarshalSource and MarshalTarget classes is permitted to vary on either side of
	//an assembly boundary, so we need to ensure our reference to the interface for this
	//object can be reliably retrieved, even if the size of the target object is different
	//to what is expected.
	const IMarshalSource<std::basic_string<ElementType, traits, Alloc>>& _sourceReference;
	MarshalSource<std::basic_string<ElementType, traits, Alloc>> _source;
};

} //Close namespace Marshal
} //Close namespace MarshalSupport
#include "MarshalInStringOperators.inl"
#endif
