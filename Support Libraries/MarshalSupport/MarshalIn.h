#ifndef __MARSHALSUPPORT_MARSHALIN_H__
#define __MARSHALSUPPORT_MARSHALIN_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalSource.h"
#include "MarshalInOut.h"
#include "MarshalRet.h"
#include "IsOnlyMovable.h"
#include "IsAssignable.h"
#include "GetLastNestedContainerElementType.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#include "NoBoundObjectTag.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#include <memory>
#endif
namespace MarshalSupport {
namespace Marshal {

//----------------------------------------------------------------------------------------
//This is the actual definition of the Marshal::In class template. Note however that this is never used directly. We
//provide specializations of this template that cover all cases.
template<class ContainerType, bool IsOnlyMovable = MarshalSupport::Internal::is_only_movable<typename MarshalSupport::Internal::get_last_nested_container_element_type<ContainerType>::type>::value, bool IsAssignable = MarshalSupport::Internal::is_assignable<ContainerType>::value, bool IsLastElement = MarshalSupport::Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = MarshalSupport::Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class In
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement, bool IsThisOrNextElementLastElement>
class In<ContainerType, false, true, IsLastElement, IsThisOrNextElementLastElement>
{
public:
	//Constructors
	inline In(const ContainerType& source)
	:_sourceReference(_source), _source(source)
	{ }
	inline In(const InOut<ContainerType>& source)
	:_sourceReference(source._sourceReference), _source((no_bound_object_tag*)0)
	{ }
	inline In(const Ret<ContainerType>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline In(ContainerType&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }
#endif

	//Marshal methods
	inline ContainerType Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline void Get(ContainerType& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalTo(targetObject);
	}
	inline ContainerType GetWithoutMove() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void GetWithoutMove(ContainerType& targetObject) const
	{
		_sourceReference.MarshalToWithoutMove(targetObject);
	}

	//Implicit conversions
	inline operator ContainerType() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than the address of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const ContainerType& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<ContainerType>& _sourceReference;
	MarshalSource<ContainerType> _source;
};

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement, bool IsThisOrNextElementLastElement>
class In<ContainerType, false, false, IsLastElement, IsThisOrNextElementLastElement>
{
public:
	//Constructors
	inline In(const ContainerType& source)
	:_sourceReference(_source), _source(source)
	{ }
	inline In(const InOut<ContainerType>& source)
	:_sourceReference(source._sourceReference), _source((no_bound_object_tag*)0)
	{ }
	inline In(const Ret<ContainerType>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline In(ContainerType&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }
#endif

	//Marshal methods
	inline ContainerType Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline ContainerType GetWithoutMove() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

	//Implicit conversions
	inline operator ContainerType() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than the address of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const ContainerType& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<ContainerType>& _sourceReference;
	MarshalSource<ContainerType> _source;
};

//----------------------------------------------------------------------------------------
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ContainerType, bool IsLastElement, bool IsThisOrNextElementLastElement>
class In<ContainerType, true, true, IsLastElement, IsThisOrNextElementLastElement>
{
public:
	//Constructors
	inline In(const Ret<ContainerType>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
	inline In(ContainerType&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }

	//Marshal methods
	inline ContainerType Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline void Get(ContainerType& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalTo(targetObject);
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than the address of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const ContainerType& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<ContainerType>& _sourceReference;
	MarshalSource<ContainerType> _source;
};

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement, bool IsThisOrNextElementLastElement>
class In<ContainerType, true, false, IsLastElement, IsThisOrNextElementLastElement>
{
public:
	//Constructors
	inline In(const Ret<ContainerType>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
	inline In(ContainerType&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }

	//Marshal methods
	inline ContainerType Get() const
	{
		return _sourceReference.MarshalTo();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than the address of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const ContainerType& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<ContainerType>& _sourceReference;
	MarshalSource<ContainerType> _source;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class In<std::vector<ElementType, Alloc>, false, true, false, true>
{
public:
	//Constructors
	inline In(const std::vector<ElementType, Alloc>& source)
	:_sourceReference(_source), _source(source)
	{ }
	inline In(const InOut<std::vector<ElementType, Alloc>>& source)
	:_sourceReference(source._sourceReference), _source((no_bound_object_tag*)0)
	{ }
	inline In(const Ret<std::vector<ElementType, Alloc>>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline In(std::vector<ElementType, Alloc>&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }
#endif

	//Marshal methods
	inline std::vector<ElementType, Alloc> Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline void Get(std::vector<ElementType, Alloc>& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalTo(targetObject);
	}
	inline std::vector<ElementType, Alloc> GetWithoutMove() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void GetWithoutMove(std::vector<ElementType, Alloc>& targetObject) const
	{
		_sourceReference.MarshalToWithoutMove(targetObject);
	}

	//Capacity methods
	inline typename std::vector<ElementType, Alloc>::size_type size() const
	{
		return _sourceReference.size();
	}
	inline typename std::vector<ElementType, Alloc>::size_type capacity() const
	{
		return _sourceReference.capacity();
	}
	inline bool empty() const
	{
		return _sourceReference.empty();
	}

	//Element access methods
	inline void GetElement(typename std::vector<ElementType, Alloc>::size_type index, ElementType& element) const
	{
		_sourceReference.GetElement(index, element);
	}

	//Implicit conversions
	inline operator std::vector<ElementType, Alloc>() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than theaddress of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const std::vector<ElementType, Alloc>& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<std::vector<ElementType, Alloc>>& _sourceReference;
	MarshalSource<std::vector<ElementType, Alloc>> _source;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class In<std::vector<ElementType, Alloc>, true, true, false, true>
{
public:
	//Constructors
	inline In(const Ret<std::vector<ElementType, Alloc>>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
	inline In(std::vector<ElementType, Alloc>&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }

	//Marshal methods
	inline std::vector<ElementType, Alloc> Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline void Get(std::vector<ElementType, Alloc>& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalTo(targetObject);
	}

	//Capacity methods
	inline typename std::vector<ElementType, Alloc>::size_type size() const
	{
		return _sourceReference.size();
	}
	inline typename std::vector<ElementType, Alloc>::size_type capacity() const
	{
		return _sourceReference.capacity();
	}
	inline bool empty() const
	{
		return _sourceReference.empty();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than the address of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const std::vector<ElementType, Alloc>& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<std::vector<ElementType, Alloc>>& _sourceReference;
	MarshalSource<std::vector<ElementType, Alloc>> _source;
};

//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class In<std::array<ElementType, ArraySize>, false, true, false, true>
{
public:
	//Constructors
	inline In(const std::array<ElementType, ArraySize>& source)
	:_sourceReference(_source), _source(source)
	{ }
	inline In(const InOut<std::array<ElementType, ArraySize>>& source)
	:_sourceReference(source._sourceReference), _source((no_bound_object_tag*)0)
	{ }
	inline In(const Ret<std::array<ElementType, ArraySize>>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
	inline In(std::array<ElementType, ArraySize>&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }

	//Marshal methods
	inline std::array<ElementType, ArraySize> Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline void Get(std::array<ElementType, ArraySize>& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalTo(targetObject);
	}
	inline std::array<ElementType, ArraySize> GetWithoutMove() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void GetWithoutMove(std::array<ElementType, ArraySize>& targetObject) const
	{
		_sourceReference.MarshalToWithoutMove(targetObject);
	}

	//Capacity methods
	inline typename std::array<ElementType, ArraySize>::size_type size() const
	{
		return _sourceReference.size();
	}
	inline bool empty() const
	{
		return _sourceReference.empty();
	}

	//Element access methods
	inline void GetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType& element) const
	{
		_sourceReference.GetElement(index, element);
	}

	//Implicit conversions
	inline operator std::array<ElementType, ArraySize>() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than theaddress of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const std::array<ElementType, ArraySize>& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<std::array<ElementType, ArraySize>>& _sourceReference;
	MarshalSource<std::array<ElementType, ArraySize>> _source;
};

//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class In<std::array<ElementType, ArraySize>, true, true, false, true>
{
public:
	//Constructors
	inline In(const Ret<std::array<ElementType, ArraySize>>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
	inline In(std::array<ElementType, ArraySize>&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }

	//Marshal methods
	inline std::array<ElementType, ArraySize> Get() const
	{
		return _sourceReference.MarshalTo();
	}
	inline void Get(std::array<ElementType, ArraySize>& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalTo(targetObject);
	}

	//Capacity methods
	inline typename std::array<ElementType, ArraySize>::size_type size() const
	{
		return _sourceReference.size();
	}
	inline bool empty() const
	{
		return _sourceReference.empty();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than the address of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const std::array<ElementType, ArraySize>& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<std::array<ElementType, ArraySize>>& _sourceReference;
	MarshalSource<std::array<ElementType, ArraySize>> _source;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class In<std::basic_string<ElementType, traits, Alloc>, false, true, false, true>
{
public:
	//Constructors
	inline In(const std::basic_string<ElementType, traits, Alloc>& source)
	:_sourceReference(_source), _source(source)
	{ }
	inline In(const InOut<std::basic_string<ElementType, traits, Alloc>>& source)
	:_sourceReference(source._sourceReference), _source((no_bound_object_tag*)0)
	{ }
	inline In(const Ret<std::basic_string<ElementType, traits, Alloc>>& source)
	:_sourceReference(source._data->GetMarshaller()), _source((no_bound_object_tag*)0)
	{ }
	inline In(const ElementType* source)
	:_sourceReference(_source), _source(source)
	{ }
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline In(std::basic_string<ElementType, traits, Alloc>&& source)
	:_sourceReference(_source), _source(std::move(source))
	{ }
#endif

	//Marshal methods
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

	//Implicit conversions
	inline operator std::basic_string<ElementType, traits, Alloc>() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference member could be initialized with a value of
	//anything other than the address of the _source member. Since this type could be derived from elsewhere, and this
	//constructor could be called by a derived type, and we explicitly set the value of _sourceReference here to an
	//externally provided value, the optimizer cannot assume what the contents of the _sourceReference member could be
	//based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	//referenced object.
	inline In(bool, const std::basic_string<ElementType, traits, Alloc>& source)
	:_sourceReference(source), _source(source)
	{ }

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<std::basic_string<ElementType, traits, Alloc>>& _sourceReference;
	MarshalSource<std::basic_string<ElementType, traits, Alloc>> _source;
};

//----------------------------------------------------------------------------------------
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class T, class Deleter>
class In<std::unique_ptr<T, Deleter>, false, true, false, true>
{
public:
	//Constructors
	inline In(std::unique_ptr<T, Deleter>&& source)
	:_deleter(source.get_deleter())
	{
		_data = source.release();
	}
	inline ~In()
	{
		if(_data != nullptr)
		{
			_deleter(_data);
		}
	}

	//Marshal methods
	inline std::unique_ptr<T, Deleter> Get() const
	{
		T* data = _data;
		_data = nullptr;
		return std::unique_ptr<T, Deleter>(data, _deleter);
	}
	inline void Get(std::unique_ptr<T, Deleter>& targetObject) const
	{
		T* data = _data;
		_data = nullptr;
		targetObject.swap(std::unique_ptr<T, Deleter>(data, _deleter));
	}

private:
	//Disable copying and moving
	In(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(const In& source) MARSHALSUPPORT_DELETEMETHOD;
	In(In&& source) MARSHALSUPPORT_DELETEMETHOD;
	In& operator=(In&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	mutable T* _data;
	Deleter _deleter;
};
#endif

} //Close namespace Marshal
} //Close namespace MarshalSupport
#include "MarshalInStringOperators.inl"
#endif
