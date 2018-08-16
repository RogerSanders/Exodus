#ifndef __MARSHALSUPPORT_MARSHALINOUT_H__
#define __MARSHALSUPPORT_MARSHALINOUT_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalSource.h"
#include "MarshalTarget.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#include "IsAssignable.h"
#endif
namespace MarshalSupport {
namespace Marshal {

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = MarshalSupport::Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = MarshalSupport::Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class InOut
{
public:
	//Friend declarations
	template<class, bool, bool, bool, bool>
	friend class In;
	template<class, bool, bool, bool>
	friend class Out;

public:
	//Constructors
	inline InOut(ContainerType& target)
	:_sourceReference(_source), _targetReference(_target), _source(target), _target(target)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		static_assert(!MarshalSupport::Internal::is_only_movable<ContainerType>::value, "Target of Marshal::InOut can't be move-only");
		static_assert(MarshalSupport::Internal::is_assignable<ContainerType>::value, "Target of Marshal::InOut must be assignable");
#endif
	}

	//Marshal methods
	inline ContainerType Get() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void Get(ContainerType& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalToWithoutMove(targetObject);
	}
	inline void Set(const ContainerType& sourceObject) const
	{
		//See notes on Get method
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const InOut& operator=(const ContainerType& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void Set(ContainerType&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const InOut& operator=(ContainerType&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}
#endif

	//Implicit conversions
	inline operator ContainerType() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference and _targetReference members could be initialized
	//with a value of anything other than the addresses of the _source and _target members. Since this type could be
	//derived from elsewhere, and this constructor could be called by a derived type, and we explicitly set the value of
	//_sourceReference and _targetReference here to an externally provided value, the optimizer cannot assume what the
	//contents of these members could be based on the other provided constructors, making it impossible to optimize away
	//virtual function calls within the referenced object.
	inline InOut(bool, ContainerType& target, const ContainerType& source)
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
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<ContainerType>& _sourceReference;
	const IMarshalTarget<ContainerType>& _targetReference;
	MarshalSource<ContainerType> _source;
	MarshalTarget<ContainerType> _target;
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class InOut<std::vector<ElementType, Alloc>, false, true>
{
public:
	//Friend declarations
	template<class, bool, bool, bool, bool>
	friend class In;
	template<class, bool, bool, bool>
	friend class Out;

public:
	//Constructors
	inline InOut(std::vector<ElementType, Alloc>& target)
	:_sourceReference(_source), _targetReference(_target), _source(target), _target(target)
	{ }

	//Marshal methods
	inline std::vector<ElementType, Alloc> Get() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void Get(std::vector<ElementType, Alloc>& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalToWithoutMove(targetObject);
	}
	inline void Set(const std::vector<ElementType, Alloc>& sourceObject) const
	{
		//See notes on Get method
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const InOut& operator=(const std::vector<ElementType, Alloc>& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void Set(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const InOut& operator=(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
	}
#endif

	//Capacity methods
	inline typename std::vector<ElementType, Alloc>::size_type size() const
	{
		return _sourceReference.size();
	}
	inline void resize(typename std::vector<ElementType, Alloc>::size_type newSize, const ElementType& insertElement = ElementType()) const
	{
		_targetReference.resize(newSize, insertElement);
	}
	inline typename std::vector<ElementType, Alloc>::size_type capacity() const
	{
		return _sourceReference.capacity();
	}
	inline bool empty() const
	{
		return _sourceReference.empty();
	}
	inline void reserve(typename std::vector<ElementType, Alloc>::size_type newSize) const
	{
		_targetReference.reserve(newSize);
	}

	//Element access methods
	inline void GetElement(typename std::vector<ElementType, Alloc>::size_type index, ElementType& element) const
	{
		_sourceReference.GetElement(index, element);
	}
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

	//Modifier methods
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

	//Implicit conversions
	inline operator std::vector<ElementType, Alloc>() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference and _targetReference members could be initialized
	//with a value of anything other than the addresses of the _source and _target members. Since this type could be
	//derived from elsewhere, and this constructor could be called by a derived type, and we explicitly set the value of
	//_sourceReference and _targetReference here to an externally provided value, the optimizer cannot assume what the
	//contents of these members could be based on the other provided constructors, making it impossible to optimize away
	//virtual function calls within the referenced object.
	inline InOut(bool, std::vector<ElementType, Alloc>& target, const std::vector<ElementType, Alloc>& source)
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
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<std::vector<ElementType, Alloc>>& _sourceReference;
	const IMarshalTarget<std::vector<ElementType, Alloc>>& _targetReference;
	MarshalSource<std::vector<ElementType, Alloc>> _source;
	MarshalTarget<std::vector<ElementType, Alloc>> _target;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class InOut<std::array<ElementType, ArraySize>, false, true>
{
public:
	//Friend declarations
	template<class, bool, bool, bool, bool>
	friend class In;
	template<class, bool, bool, bool>
	friend class Out;

public:
	//Constructors
	inline InOut(std::array<ElementType, ArraySize>& target)
	:_sourceReference(_source), _targetReference(_target), _source(target), _target(target)
	{ }

	//Marshal methods
	inline std::array<ElementType, ArraySize> Get() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}
	inline void Get(std::array<ElementType, ArraySize>& targetObject) const
	{
		//Note that we use a reference to the marshaller object here. This is done to ensure that the compiler can't
		//optimize away any virtual function calls we're using to enforce code boundaries between assemblies. If the
		//compiler could be sure of the actual type of the target marshaller object, it could attempt to do this. Since
		//this class is designed to be passed by reference across assembly boundaries, the compiler can't assume
		//anything about the existing contents of the object when it is passed as an argument to a method in the target
		//assembly, and therefore cannot optimize away our virtual function boundaries.
		_sourceReference.MarshalToWithoutMove(targetObject);
	}
	inline void Set(const std::array<ElementType, ArraySize>& sourceObject) const
	{
		//See notes on Get method
		_targetReference.MarshalFrom(sourceObject);
	}
	inline const InOut& operator=(const std::array<ElementType, ArraySize>& sourceObject) const
	{
		_targetReference.MarshalFrom(sourceObject);
		return *this;
	}
	inline void Set(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
	}
	inline const InOut& operator=(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		_targetReference.MarshalFrom(std::move(sourceObject));
		return *this;
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
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, const ElementType& element) const
	{
		_targetReference.SetElement(index, element);
	}
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType&& element) const
	{
		_targetReference.SetElement(index, std::move(element));
	}

	//Modifier methods
	inline void AssignFrom(const ElementType* data) const
	{
		_targetReference.AssignFrom(data);
	}
	inline void AssignFromWithMove(ElementType* data) const
	{
		_targetReference.AssignFromWithMove(data);
	}

	//Implicit conversions
	inline operator std::array<ElementType, ArraySize>() const
	{
		return _sourceReference.MarshalToWithoutMove();
	}

protected:
	//This protected constructor is never actually used by anything, nor should it ever be. The only purpose of this
	//constructor is to introduce a path by which the _sourceReference and _targetReference members could be initialized
	//with a value of anything other than the addresses of the _source and _target members. Since this type could be
	//derived from elsewhere, and this constructor could be called by a derived type, and we explicitly set the value of
	//_sourceReference and _targetReference here to an externally provided value, the optimizer cannot assume what the
	//contents of these members could be based on the other provided constructors, making it impossible to optimize away
	//virtual function calls within the referenced object.
	inline InOut(bool, std::array<ElementType, ArraySize>& target, const std::array<ElementType, ArraySize>& source)
	:_sourceReference(source), _targetReference(target), _source(source), _target(target)
	{ }

private:
	//Disable copying and moving
	InOut(const InOut& sourceObject) MARSHALSUPPORT_DELETEMETHOD;
	InOut& operator=(const InOut& source) MARSHALSUPPORT_DELETEMETHOD;
	InOut(InOut&& sourceObject) MARSHALSUPPORT_DELETEMETHOD;
	InOut& operator=(InOut&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	//Note that these data members MUST be listed in this order. The layout and size of the MarshalSource and
	//MarshalTarget classes is permitted to vary on either side of an assembly boundary, so we need to ensure our
	//reference to the interface for this object can be reliably retrieved, even if the size of the target object is
	//different to what is expected.
	const IMarshalSource<std::array<ElementType, ArraySize>>& _sourceReference;
	const IMarshalTarget<std::array<ElementType, ArraySize>>& _targetReference;
	MarshalSource<std::array<ElementType, ArraySize>> _source;
	MarshalTarget<std::array<ElementType, ArraySize>> _target;
};
#endif

} //Close namespace Marshal
} //Close namespace MarshalSupport
#include "MarshalInOutStringOperators.inl"
#endif
