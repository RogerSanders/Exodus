#ifndef __MARSHALSUPPORT_MARSHALOUT_H__
#define __MARSHALSUPPORT_MARSHALOUT_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalTarget.h"
#include "MarshalInOut.h"
#include "IsOnlyMovable.h"
#include "GetLastNestedContainerElementType.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#include "NoBoundObjectTag.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#include <memory>
#include "IsAssignable.h"
#endif
namespace MarshalSupport {
namespace Marshal {

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType, bool IsOnlyMovable = MarshalSupport::Internal::is_only_movable<typename MarshalSupport::Internal::get_last_nested_container_element_type<ContainerType>::type>::value, bool IsLastElement = MarshalSupport::Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = MarshalSupport::Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class Out
{
public:
	// Constructors
	inline Out(ContainerType& target)
	:_targetReference(_target), _target(target)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		static_assert(MarshalSupport::Internal::is_assignable<ContainerType>::value, "Target of Marshal::Out must be assignable");
#endif
	}
	inline Out(const InOut<ContainerType>& target)
	:_targetReference(target._targetReference), _target((no_bound_object_tag*)0)
	{ }

	// Marshal methods
	inline void Set(const ContainerType& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const Out& operator=(const ContainerType& sourceObject) const
	{
		// Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		// optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		// compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		// this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		// anything about the existing contents of the object when it is passed as an argument to a method in the target
		// assembly, and therefore cannot optimize away our virtual function boundaries.
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void Set(ContainerType&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const Out& operator=(ContainerType&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}
#endif

protected:
	// This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	// constructor is to introduce a path by which the _targetReference member could be initialized with a value of
	// anything other than the address of the _target member. Since this type could be derived from elsewhere, and this
	// constructor could be called by a derived type, and we explicitly set the value of _targetReference here to an
	// externally provided value, the optimizer cannot assume what the contents of the _targetReference member could be
	// based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	// referenced object.
	inline Out(bool, ContainerType& target)
	:_targetReference(target), _target(target)
	{ }

private:
	// Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	// Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	// MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	// reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	// different to what is expected.
	const IMarshalTarget<ContainerType>& _targetReference;
	MarshalTarget<ContainerType> _target;
};

//----------------------------------------------------------------------------------------------------------------------
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ContainerType, bool IsLastElement, bool IsThisOrNextElementLastElement>
class Out<ContainerType, true, IsLastElement, IsThisOrNextElementLastElement>
{
public:
	// Constructors
	inline Out(ContainerType& target)
	:_targetReference(_target), _target(target)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		static_assert(MarshalSupport::Internal::is_assignable<ContainerType>::value, "Target of Marshal::Out must be assignable");
#endif
	}
	inline Out(const InOut<ContainerType>& target)
	:_targetReference(target._targetReference), _target((no_bound_object_tag*)0)
	{ }

	// Marshal methods
	inline void Set(ContainerType&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const Out& operator=(ContainerType&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}

protected:
	// This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	// constructor is to introduce a path by which the _targetReference member could be initialized with a value of
	// anything other than the address of the _target member. Since this type could be derived from elsewhere, and this
	// constructor could be called by a derived type, and we explicitly set the value of _targetReference here to an
	// externally provided value, the optimizer cannot assume what the contents of the _targetReference member could be
	// based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	// referenced object.
	inline Out(bool, ContainerType& target)
	:_targetReference(target), _target(target)
	{ }

private:
	// Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	// Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	// MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	// reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	// different to what is expected.
	const IMarshalTarget<ContainerType>& _targetReference;
	MarshalTarget<ContainerType> _target;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class Out<std::vector<ElementType, Alloc>, false, false, true>
{
public:
	// Constructors
	inline Out(std::vector<ElementType, Alloc>& target)
	:_targetReference(_target), _target(target)
	{ }
	inline Out(const InOut<std::vector<ElementType, Alloc>>& target)
	:_targetReference(target._targetReference), _target((no_bound_object_tag*)0)
	{ }

	// Marshal methods
	inline void Set(const std::vector<ElementType, Alloc>& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const Out& operator=(const std::vector<ElementType, Alloc>& sourceObject) const
	{
		// Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		// optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		// compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		// this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		// anything about the existing contents of the object when it is passed as an argument to a method in the target
		// assembly, and therefore cannot optimize away our virtual function boundaries.
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void Set(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const Out& operator=(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}
#endif

	// Capacity methods
	inline void resize(typename std::vector<ElementType, Alloc>::size_type newSize, const ElementType& insertElement = ElementType()) const
	{
		_targetReference.resize(newSize, insertElement);
	}
	inline void reserve(typename std::vector<ElementType, Alloc>::size_type newSize) const
	{
		_targetReference.reserve(newSize);
	}

	// Element access methods
	inline void SetElement(typename std::vector<ElementType, Alloc>::size_type index, const ElementType& element) const
	{
		_targetReference.SetElement(index, element);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void SetElement(typename std::vector<ElementType, Alloc>::size_type index, ElementType&& element) const
	{
		_targetReference.SetElement(index, std::move(element));
	}
#endif

	// Modifier methods
	inline void push_back(const ElementType& element) const
	{
		_targetReference.push_back(element);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void push_back(ElementType&& element) const
	{
		_targetReference.push_back(std::move(element));
	}
#endif
	inline void pop_back() const
	{
		_targetReference.pop_back();
	}
	inline void clear() const
	{
		_targetReference.clear();
	}
	inline void assign(typename std::vector<ElementType, Alloc>::size_type newSize, const ElementType& element) const
	{
		_targetReference.assign(newSize, element);
	}
	inline void AssignFrom(const ElementType* data, size_t count) const
	{
		_targetReference.AssignFrom(data, count);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void AssignFromWithMove(ElementType* data, size_t count) const
	{
		_targetReference.AssignFromWithMove(data, count);
	}
#endif

protected:
	// This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	// constructor is to introduce a path by which the _targetReference member could be initialized with a value of
	// anything other than the address of the _target member. Since this type could be derived from elsewhere, and this
	// constructor could be called by a derived type, and we explicitly set the value of _targetReference here to an
	// externally provided value, the optimizer cannot assume what the contents of the _targetReference member could be
	// based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	// referenced object.
	inline Out(bool, std::vector<ElementType, Alloc>& target)
	:_targetReference(target), _target(target)
	{ }

private:
	// Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	// Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	// MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	// reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	// different to what is expected.
	const IMarshalTarget<std::vector<ElementType, Alloc>>& _targetReference;
	MarshalTarget<std::vector<ElementType, Alloc>> _target;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class Out<std::vector<ElementType, Alloc>, true, false, true>
{
public:
	// Constructors
	inline Out(std::vector<ElementType, Alloc>& target)
	:_targetReference(_target), _target(target)
	{ }
	inline Out(const InOut<std::vector<ElementType, Alloc>>& target)
	:_targetReference(target._targetReference), _target((no_bound_object_tag*)0)
	{ }

	// Marshal methods
	inline void Set(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const Out& operator=(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}

	// Capacity methods
	inline void reserve(typename std::vector<ElementType, Alloc>::size_type newSize) const
	{
		_targetReference.reserve(newSize);
	}

	// Element access methods
	inline void SetElement(typename std::vector<ElementType, Alloc>::size_type index, ElementType&& element) const
	{
		_targetReference.SetElement(index, std::move(element));
	}

	// Modifier methods
	inline void push_back(ElementType&& element) const
	{
		_targetReference.push_back(std::move(element));
	}
	inline void pop_back() const
	{
		_targetReference.pop_back();
	}
	inline void clear() const
	{
		_targetReference.clear();
	}
	inline void AssignFromWithMove(ElementType* data, size_t count) const
	{
		_targetReference.AssignFromWithMove(data, count);
	}

protected:
	// This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	// constructor is to introduce a path by which the _targetReference member could be initialized with a value of
	// anything other than the address of the _target member. Since this type could be derived from elsewhere, and this
	// constructor could be called by a derived type, and we explicitly set the value of _targetReference here to an
	// externally provided value, the optimizer cannot assume what the contents of the _targetReference member could be
	// based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	// referenced object.
	inline Out(bool, std::vector<ElementType, Alloc>& target)
	:_targetReference(target), _target(target)
	{ }

private:
	// Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	// Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	// MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	// reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	// different to what is expected.
	const IMarshalTarget<std::vector<ElementType, Alloc>>& _targetReference;
	MarshalTarget<std::vector<ElementType, Alloc>> _target;
};

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class Out<std::array<ElementType, ArraySize>, false, false, true>
{
public:
	// Constructors
	inline Out(std::array<ElementType, ArraySize>& target)
	:_targetReference(_target), _target(target)
	{ }
	inline Out(const InOut<std::array<ElementType, ArraySize>>& target)
	:_targetReference(target._targetReference), _target((no_bound_object_tag*)0)
	{ }

	// Marshal methods
	inline void Set(const std::array<ElementType, ArraySize>& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const Out& operator=(const std::array<ElementType, ArraySize>& sourceObject) const
	{
		// Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		// optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		// compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		// this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		// anything about the existing contents of the object when it is passed as an argument to a method in the target
		// assembly, and therefore cannot optimize away our virtual function boundaries.
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}
	inline void Set(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const Out& operator=(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}

	// Element access methods
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, const ElementType& element) const
	{
		_targetReference.SetElement(index, element);
	}
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType&& element) const
	{
		_targetReference.SetElement(index, std::move(element));
	}

	// Modifier methods
	inline void AssignFrom(const ElementType* data) const
	{
		_targetReference.AssignFrom(data);
	}
	inline void AssignFromWithMove(ElementType* data) const
	{
		_targetReference.AssignFromWithMove(data);
	}

protected:
	// This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	// constructor is to introduce a path by which the _targetReference member could be initialized with a value of
	// anything other than the address of the _target member. Since this type could be derived from elsewhere, and this
	// constructor could be called by a derived type, and we explicitly set the value of _targetReference here to an
	// externally provided value, the optimizer cannot assume what the contents of the _targetReference member could be
	// based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	// referenced object.
	inline Out(bool, std::array<ElementType, ArraySize>& target)
	:_targetReference(target), _target(target)
	{ }

private:
	// Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	// Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	// MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	// reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	// different to what is expected.
	const IMarshalTarget<std::array<ElementType, ArraySize>>& _targetReference;
	MarshalTarget<std::array<ElementType, ArraySize>> _target;
};

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class Out<std::array<ElementType, ArraySize>, true, false, true>
{
public:
	// Constructors
	inline Out(std::array<ElementType, ArraySize>& target)
	:_targetReference(_target), _target(target)
	{ }
	inline Out(const InOut<std::array<ElementType, ArraySize>>& target)
	:_targetReference(target._targetReference), _target((no_bound_object_tag*)0)
	{ }

	// Marshal methods
	inline void Set(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const Out& operator=(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}

	// Element access methods
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType&& element) const
	{
		_targetReference.SetElement(index, std::move(element));
	}

	// Modifier methods
	inline void AssignFromWithMove(ElementType* data) const
	{
		_targetReference.AssignFromWithMove(data);
	}

protected:
	// This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	// constructor is to introduce a path by which the _targetReference member could be initialized with a value of
	// anything other than the address of the _target member. Since this type could be derived from elsewhere, and this
	// constructor could be called by a derived type, and we explicitly set the value of _targetReference here to an
	// externally provided value, the optimizer cannot assume what the contents of the _targetReference member could be
	// based on the other provided constructors, making it impossible to optimize away virtual function calls within the
	// referenced object.
	inline Out(bool, std::array<ElementType, ArraySize>& target)
	:_targetReference(target), _target(target)
	{ }

private:
	// Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	// Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	// MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	// reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	// different to what is expected.
	const IMarshalTarget<std::array<ElementType, ArraySize>>& _targetReference;
	MarshalTarget<std::array<ElementType, ArraySize>> _target;
};

//----------------------------------------------------------------------------------------------------------------------
template<class T, class Deleter>
class Out<std::unique_ptr<T, Deleter>, false, false, true>
{
public:
	// Constructors
	inline Out(std::unique_ptr<T, Deleter>& target)
	:_targetReference(_target), _target(target)
	{ }

	// Marshal methods
	inline void Set(std::unique_ptr<T, Deleter>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline void Set(T* sourceObject) const
	{
		_targetReference.MarshalFrom(std::unique_ptr<T, Deleter>(sourceObject));
	}
	inline const Out& operator=(std::unique_ptr<T, Deleter>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}
	inline const Out& operator=(T* sourceObject) const
	{
		_targetReference.MarshalFrom(std::unique_ptr<T, Deleter>(sourceObject));
		return *this;
	}

private:
	// Disable copying and moving
	Out(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(const Out& source) MARSHALSUPPORT_DELETEMETHOD;
	Out(Out&& source) MARSHALSUPPORT_DELETEMETHOD;
	Out& operator=(Out&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	const IMarshalTarget<std::unique_ptr<T, Deleter>>& _targetReference;
	MarshalTarget<std::unique_ptr<T, Deleter>> _target;
};
#endif

} // Close namespace Marshal
} // Close namespace MarshalSupport
#endif
