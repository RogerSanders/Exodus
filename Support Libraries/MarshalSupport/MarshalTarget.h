#ifndef __MARSHALTARGET_H__
#define __MARSHALTARGET_H__
#include "IMarshalTarget.h"
#include "MarshalPreprocessorMacros.h"
#include "IsOnlyMovable.h"
#include "GetLastNestedContainerElementType.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#include "IsSTLContainerNestedElementMarshallableOrSameSize.h"
#include "EnsureSTLContainerKeySizesMatch.h"
#include "HasMarshalConstructor.h"
#include "ClearSTLContainer.h"
#include "MarshalObjectHelper.h"
#include "RecomposeSTLContainer.h"
#include "NoBoundObjectTag.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <memory.h>
#endif
#include <cassert>
namespace MarshalSupport {

#ifdef _MSC_VER
#pragma warning(push)
// Disable warning about using "this" pointer in initializer list. Our usage here is safe, and guaranteed by the
// standard. See section 12.6.2 [class.base.init] paragraph 7 of the standard for clarification.
#pragma warning(disable:4355)
// Disable warning about our use of type traits causing conditional expressions to be constant. The code behaves as
// intended, and the compiler is free to optimize away the dead branch.
#pragma warning(disable:4127)
// Disable warning about classes with virtual functions without a virtual destructor. We use protected inlined
// destructors where appropriate to prevent destruction through base class pointers.
#pragma warning(disable:4265)
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-virtual-dtor"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace Internal {
template<class ContainerType, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
class MarshalTargetInternal :public IMarshalTarget<ContainerType, false, IsThisOrNextElementLastElement, IsOnlyMovable>
{
public:
	// Constructors
	MarshalTargetInternal(ContainerType& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTargetInternal(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFromFlatArrays(size_t elementByteSize, void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(nestedMarshallerArray, 0, (ContainerType*)0);

		// Ensure the target container is cleared of any current data
		Internal::ClearSTLContainer(*_targetObject);

		// Recompose our flat arrays into the target container
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, *_targetObject);
	}

private:
	// Disable copying and moving
	MarshalTargetInternal(const MarshalTargetInternal& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTargetInternal& operator=(const MarshalTargetInternal& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTargetInternal(MarshalTargetInternal&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTargetInternal& operator=(MarshalTargetInternal&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	ContainerType* _targetObject;
};
} // Close namespace Internal

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
class MarshalTarget
{ };

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType, bool IsThisOrNextElementLastElement, bool IsOnlyMovable>
class MarshalTarget<ContainerType, false, IsThisOrNextElementLastElement, IsOnlyMovable> :public Internal::MarshalTargetInternal<ContainerType, IsThisOrNextElementLastElement, IsOnlyMovable>
{
public:
	// Constructors
	MarshalTarget(ContainerType& targetObject)
	:Internal::MarshalTargetInternal<ContainerType, IsThisOrNextElementLastElement>(targetObject)
	{ }
	MarshalTarget(no_bound_object_tag* tag)
	:Internal::MarshalTargetInternal<ContainerType, IsThisOrNextElementLastElement>(tag)
	{ }

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalTarget<ContainerType, true, true, false> :public IMarshalTarget<ContainerType, true, true, false>
{
public:
	// Constructors
	MarshalTarget(ContainerType& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalDirectFromSource(size_t elementByteSize, const ContainerType& sourceObject) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Marshal the object, performing a copy, or invoking a marshal constructor as required.
		Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(sourceObject, *_targetObject);
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalDirectFromSource(size_t elementByteSize, ContainerType& sourceObject) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Marshal the object, performing a move, or invoking a marshal constructor as required.
		Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(sourceObject), *_targetObject);
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	ContainerType* _targetObject;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalTarget<ContainerType, true, true, true> :public IMarshalTarget<ContainerType, true, true, true>
{
public:
	// Constructors
	MarshalTarget(ContainerType& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalDirectFromSource(size_t elementByteSize, ContainerType&& sourceObject) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Marshal the object, performing a move, or invoking a marshal constructor as required.
		Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(std::move(sourceObject), *_targetObject);
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	ContainerType* _targetObject;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalTarget<std::vector<ElementType, Alloc>, false, true, false> :public IMarshalTarget<std::vector<ElementType, Alloc>, false, true, false>
{
public:
	// Constructors
	MarshalTarget(std::vector<ElementType, Alloc>& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			_targetObject->clear();
			_targetObject->reserve(sourceDataLength);

			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(*nextSourceObject, *_targetObject);
				nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data
			_targetObject->assign(sourceData, sourceData + sourceDataLength);
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			_targetObject->clear();
			_targetObject->reserve(sourceDataLength);

			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), *_targetObject);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data. Note that we only perform the
			// assignment using move iterators where the target elements use a non-trivial type. This is done as an
			// optimization. Although the assign method in theory has all the necessary information to optimize copying
			// an array of standard layout types in one process using memcpy, in VS2013 it was observed that the move
			// iterator wrappers prevent this optimization occurring. We avoid using move iterators in this case to
			// achieve maximum efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if (!std::is_trivial<ElementType>::value)
			{
				_targetObject->assign(std::make_move_iterator(const_cast<ElementType*>(sourceData)), std::make_move_iterator(const_cast<ElementType*>(sourceData + sourceDataLength)));
			}
			else
#endif
			{
				_targetObject->assign(sourceData, sourceData + sourceDataLength);
			}
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Marshal the object
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*sourceData, (*_targetObject)[(typename std::vector<ElementType, Alloc>::size_type)elementIndex]);
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Marshal the object
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*sourceData), (*_targetObject)[(typename std::vector<ElementType, Alloc>::size_type)elementIndex]);
#endif
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalPushBack(size_t elementByteSize, const ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Marshal the object
		Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(*sourceData, *_targetObject);
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalPushBack(size_t elementByteSize, ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Marshal the object
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(std::move(*sourceData), *_targetObject);
#endif
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			ElementType* nextTargetObject = _targetObject->data() + elementIndex;
#else
			ElementType* nextTargetObject = (_targetObject->empty() ? 0 : &(*_targetObject)[(typename std::vector<ElementType, Alloc>::size_type)elementIndex]);
#endif
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*nextSourceObject, *(nextTargetObject++));
				nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		else if (!std::is_trivial<ElementType>::value)
#else
		else
#endif
		{
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				(*_targetObject)[(typename std::vector<ElementType, Alloc>::size_type)(elementIndex + i)] = sourceData[i];
			}
		}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		else
		{
			memcpy((void*)(_targetObject->data() + elementIndex), (const void*)sourceData, sourceDataLength * sizeof(ElementType));
		}
#endif
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			ElementType* nextTargetObject = _targetObject->data() + elementIndex;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(*nextSourceObject), *(nextTargetObject++));
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else if (!std::is_trivial<ElementType>::value)
		{
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				(*_targetObject)[(typename std::vector<ElementType, Alloc>::size_type)(elementIndex + i)] = std::move(sourceData[i]);
			}
		}
		else
		{
			memcpy((void*)(_targetObject->data() + elementIndex), (const void*)sourceData, sourceDataLength * sizeof(ElementType));
		}
#endif
	}

	// Size methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION Clear() const
	{
		_targetObject->clear();
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION PopBack() const
	{
		_targetObject->pop_back();
	}
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const
	{
		return static_cast<size_t>(_targetObject->capacity());
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION SetReservedSize(size_t newSize) const
	{
		_targetObject->reserve(static_cast<typename std::vector<ElementType, Alloc>::size_type>(newSize));
	}
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const
	{
		return static_cast<size_t>(_targetObject->size());
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION SetSize(size_t newSize, const ElementType& insertElement) const
	{
		_targetObject->resize(static_cast<typename std::vector<ElementType, Alloc>::size_type>(newSize), Internal::MarshalObjectHelper<ElementType>::MarshalObjectToNewObject(insertElement));
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	std::vector<ElementType, Alloc>* _targetObject;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalTarget<std::vector<ElementType, Alloc>, false, true, true> :public IMarshalTarget<std::vector<ElementType, Alloc>, false, true, true>
{
public:
	// Constructors
	MarshalTarget(std::vector<ElementType, Alloc>& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			_targetObject->clear();
			_targetObject->reserve(sourceDataLength);

			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(std::move(*nextSourceObject), *_targetObject);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data
			_targetObject->assign(std::make_move_iterator(sourceData), std::make_move_iterator(sourceData + sourceDataLength));
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Marshal the object
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*sourceData), (*_targetObject)[(typename std::vector<ElementType, Alloc>::size_type)elementIndex]);
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalPushBack(size_t elementByteSize, ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Marshal the object
		Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(std::move(*sourceData), *_targetObject);
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			ElementType* nextTargetObject = _targetObject->data() + elementIndex;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(*nextSourceObject), *(nextTargetObject++));
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else if (!std::is_trivial<ElementType>::value)
		{
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				(*_targetObject)[(typename std::vector<ElementType, Alloc>::size_type)(elementIndex + i)] = std::move(sourceData[i]);
			}
		}
		else
		{
			memcpy((void*)(_targetObject->data() + elementIndex), (const void*)sourceData, sourceDataLength * sizeof(ElementType));
		}
	}

	// Size methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION Clear() const
	{
		_targetObject->clear();
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION PopBack() const
	{
		_targetObject->pop_back();
	}
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const
	{
		return static_cast<size_t>(_targetObject->capacity());
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION SetReservedSize(size_t newSize) const
	{
		_targetObject->reserve(static_cast<typename std::vector<ElementType, Alloc>::size_type>(newSize));
	}
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const
	{
		return static_cast<size_t>(_targetObject->size());
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::vector<ElementType, Alloc>* _targetObject;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class Alloc>
class MarshalTarget<std::vector<bool, Alloc>, false, true, false> :public IMarshalTarget<std::vector<bool, Alloc>, false, true, false>
{
public:
	// Constructors
	MarshalTarget(std::vector<bool, Alloc>& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(const bool* sourceData, size_t sourceDataLength) const
	{
		_targetObject->assign(sourceData, sourceData + sourceDataLength);
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	std::vector<bool, Alloc>* _targetObject;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class MarshalTarget<std::array<ElementType, ArraySize>, false, true, false> :public IMarshalTarget<std::array<ElementType, ArraySize>, false, true, false>
{
public:
	// Constructors
	MarshalTarget(std::array<ElementType, ArraySize>& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Ensure supplied element index and array size fit within the size of the container
		assert((elementIndex >= 0) && ((elementIndex + sourceDataLength) <= ArraySize));

		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal each element of the supplied array to the target array. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			size_t elementIndexEnd = std::min(elementIndex + sourceDataLength, ArraySize);
			const ElementType* nextSourceObject = sourceData;
			for (size_t i = elementIndex; i < elementIndexEnd; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*nextSourceObject, (*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)i]);
				nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else if (!std::is_trivial<ElementType>::value)
		{
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				(*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)(elementIndex + i)] = sourceData[i];
			}
		}
		else
		{
			memcpy((void*)(_targetObject->data() + elementIndex), (const void*)sourceData, sourceDataLength * sizeof(ElementType));
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Ensure supplied element index and array size fit within the size of the container
		assert((elementIndex >= 0) && ((elementIndex + sourceDataLength) <= ArraySize));

		// Either marshal or move the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal each element of the supplied array to the target array. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			size_t elementIndexEnd = std::min(elementIndex + sourceDataLength, ArraySize);
			ElementType* nextSourceObject = sourceData;
			for (size_t i = elementIndex; i < elementIndexEnd; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*nextSourceObject), (*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)i]);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else if (!std::is_trivial<ElementType>::value)
		{
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				(*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)(elementIndex + i)] = std::move(sourceData[i]);
			}
		}
		else
		{
			memcpy((void*)(_targetObject->data() + elementIndex), (const void*)sourceData, sourceDataLength * sizeof(ElementType));
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Marshal the object
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*sourceData, (*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)elementIndex]);
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Marshal the object
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*sourceData), (*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)elementIndex]);
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::array<ElementType, ArraySize>* _targetObject;
};

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class MarshalTarget<std::array<ElementType, ArraySize>, false, true, true> :public IMarshalTarget<std::array<ElementType, ArraySize>, false, true, true>
{
public:
	// Constructors
	MarshalTarget(std::array<ElementType, ArraySize>& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Ensure supplied element index and array size fit within the size of the container
		assert((elementIndex >= 0) && ((elementIndex + sourceDataLength) <= ArraySize));

		// Either marshal or move the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal each element of the supplied array to the target array. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			size_t elementIndexEnd = std::min(elementIndex + sourceDataLength, ArraySize);
			ElementType* nextSourceObject = sourceData;
			for (size_t i = elementIndex; i < elementIndexEnd; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*nextSourceObject), (*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)i]);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				(*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)(elementIndex + i)] = std::move(sourceData[i]);
			}
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Marshal the object
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*sourceData), (*_targetObject)[(typename std::array<ElementType, ArraySize>::size_type)elementIndex]);
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::array<ElementType, ArraySize>* _targetObject;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class MarshalTarget<std::basic_string<ElementType, traits, Alloc>, false, true, false> :public IMarshalTarget<std::basic_string<ElementType, traits, Alloc>, false, true, false>
{
public:
	// Constructors
	MarshalTarget(std::basic_string<ElementType, traits, Alloc>& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::basic_string<ElementType, traits, Alloc>*)0));

		// Either marshal or copy the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			_targetObject->clear();
			_targetObject->reserve(sourceDataLength);

			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(*nextSourceObject, *_targetObject);
				nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data
			_targetObject->assign(sourceData, sourceData + sourceDataLength);
		}
	}
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const
	{
		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::basic_string<ElementType, traits, Alloc>*)0));

		// Either marshal or move the elements to the target array depending on whether a custom marshaller is
		// implemented for the target elements.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			_targetObject->clear();
			_targetObject->reserve(sourceDataLength);

			// Marshal each element of the supplied array to the target vector. Note that since the object has a custom
			// marshal constructor, it's possible the size and layout of the object differs on each side of the boundary.
			// We've received a pointer to an array of source objects to marshal, but we can't index that array using our
			// size of the object here, since it may differ from that of the sender. To account for this, we calculate
			// the address of each object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), *_targetObject);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data. Note that we only perform the
			// assignment using move iterators where the target elements use a non-trivial type. This is done as an
			// optimization. Although the assign method in theory has all the necessary information to optimize copying
			// an array of standard layout types in one process using memcpy, in VS2013 it was observed that the move
			// iterator wrappers prevent this optimization occurring. We avoid using move iterators in this case to
			// achieve maximum efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if (!std::is_trivial<ElementType>::value)
			{
				_targetObject->assign(std::make_move_iterator(sourceData), std::make_move_iterator(sourceData + sourceDataLength));
			}
			else
#endif
			{
				_targetObject->assign(sourceData, sourceData + sourceDataLength);
			}
		}
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	std::basic_string<ElementType, traits, Alloc>* _targetObject;
};

//----------------------------------------------------------------------------------------------------------------------
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class T, class Deleter>
class MarshalTarget<std::unique_ptr<T, Deleter>, false, true, false> :public IMarshalTarget<std::unique_ptr<T, Deleter>, false, true, false>
{
public:
	// Constructors
	MarshalTarget(std::unique_ptr<T, Deleter>& targetObject)
	:_targetObject(&targetObject)
	{ }
	MarshalTarget(no_bound_object_tag*)
	{ }

protected:
	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(T* data, const Deleter& deleter) const
	{
		std::unique_ptr<T, Deleter> temp(data, deleter);
		_targetObject->swap(temp);
	}

private:
	// Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::unique_ptr<T, Deleter>* _targetObject;
};
#endif

// Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

} // Close namespace MarshalSupport
#endif
