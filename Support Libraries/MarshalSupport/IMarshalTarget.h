#ifndef __IMARSHALTARGET_H__
#define __IMARSHALTARGET_H__
#include "MarshalPreprocessorMacros.h"
#include "IsOnlyMovable.h"
#include "GetLastNestedContainerElementType.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#include "NestedContainerHasKeys.h"
#include "CalculateDecomposedSTLContainerSize.h"
#include "CalculateSTLContainerNestingDepth.h"
#include "GetLastNestedContainerElementType.h"
#include "IsNonConstRValueReference.h"
#include "CreateSTLContainerKeyMarshallers.h"
#include "DeleteSTLContainerKeyMarshallers.h"
#include "CreateSTLContainerItemArray.h"
#include "DeleteSTLContainerItemArray.h"
#include "DecomposeSTLContainer.h"
#include "INestedMarshallerBase.h"
#include <vector>
#include <string>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#include <memory>
#endif
namespace MarshalSupport {

#ifdef _MSC_VER
#pragma warning(push)
// Disable warning about conditional expressions being constant. The code behaves as intended, and the compiler is free
// to optimize away any dead code branches.
#pragma warning(disable:4127)
// Disable warning about classes with virtual functions without a virtual destructor. We use protected inlined
// destructors where appropriate to prevent destruction through base class pointers.
#pragma warning(disable:4265)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace Internal {
template<class ContainerType, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
class IMarshalTargetInternal
{
public:
	// Marshalling methods
	inline void MarshalFrom(const ContainerType& sourceObject) const
	{
		MarshalFromInternal(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void MarshalFrom(ContainerType&& sourceObject) const
	{
		MarshalFromInternal(std::move(sourceObject));
	}
#endif

protected:
	// Constructors
	inline ~IMarshalTargetInternal() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFromFlatArrays(size_t elementByteSize, void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const = 0;

private:
	// Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		// Constructors
		ScopedFreeDataArrayHelper(const IMarshalTargetInternal& atargetMarshaller)
		:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), nestedMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			// Release the allocated data arrays
			Internal::DeleteSTLContainerItemArray(itemArray, (ContainerType*)0);
			delete[] elementSizeArray;
			if (Internal::nested_container_has_keys<ContainerType>::value)
			{
				Internal::DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, 0, (ContainerType*)0);
				delete[] nestedMarshallerArray;
			}
		}

	public:
		const IMarshalTargetInternal& targetMarshaller;
		void* itemArray;
		size_t* elementSizeArray;
		Internal::INestedMarshallerBase** nestedMarshallerArray;
	};
#ifndef MARSHALSUPPORT_CPP11SUPPORTED
	// This friend declaration is not required in C++11, but is added for earlier versions of the C++ standard due to
	// ambiguity and contradiction in prior versions regarding nested class access to its enclosing class.
	friend class ScopedFreeDataArrayHelper;
#endif

private:
	// Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		// Calculate the required size of our flat arrays in order to hold a decomposed version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		Internal::CalculateDecomposedSTLContainerSize(itemArraySize, elementSizeArraySize, sourceObject);

		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		arrayHelper.itemArray = Internal::CreateSTLContainerItemArray(itemArraySize, &sourceObject);
		arrayHelper.elementSizeArray = new size_t[elementSizeArraySize];

		// Allocate key marshallers for any keyed containers in the source object
		if (Internal::nested_container_has_keys<ContainerType>::value)
		{
			MARSHALSUPPORT_CONSTEXPR size_t nestingDepth = Internal::CalculateSTLContainerNestingDepth(0, (ContainerType*)0);
			arrayHelper.nestedMarshallerArray = new Internal::INestedMarshallerBase*[nestingDepth];
			Internal::CreateSTLContainerKeyMarshallers(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);
		}

		// Decompose the container hierarchy into a flat array of the innermost elements, with a corresponding array of
		// sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t nestedMarshallerArrayIndex = 0;
		Internal::DecomposeSTLContainer(arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, MARSHALSUPPORT_FORWARD(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType), sourceObject));

		// Recompose our flat arrays into the target container
		MarshalFromFlatArrays(sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type), arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray);
	}
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalTargetInternal<ContainerType, true>
{
public:
	// Marshalling methods
	inline void MarshalFrom(ContainerType&& sourceObject) const
	{
		// Calculate the required size of our flat arrays in order to hold a decomposed version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		Internal::CalculateDecomposedSTLContainerSize(itemArraySize, elementSizeArraySize, sourceObject);

		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		arrayHelper.itemArray = Internal::CreateSTLContainerItemArray(itemArraySize, &sourceObject);
		arrayHelper.elementSizeArray = new size_t[elementSizeArraySize];

		// Allocate key marshallers for any keyed containers in the source object
		if (Internal::nested_container_has_keys<ContainerType>::value)
		{
			MARSHALSUPPORT_CONSTEXPR size_t nestingDepth = Internal::CalculateSTLContainerNestingDepth(0, (ContainerType*)0);
			arrayHelper.nestedMarshallerArray = new Internal::INestedMarshallerBase*[nestingDepth];
			Internal::CreateSTLContainerKeyMarshallers(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);
		}

		// Decompose the container hierarchy into a flat array of the innermost elements, with a corresponding array of
		// sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t nestedMarshallerArrayIndex = 0;
		Internal::DecomposeSTLContainer(arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, std::move(sourceObject));

		// Recompose our flat arrays into the target container
		MarshalFromFlatArrays(sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type), arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray);
	}

protected:
	// Constructors
	inline ~IMarshalTargetInternal() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFromFlatArrays(size_t elementByteSize, void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const = 0;

private:
	// Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		// Constructors
		ScopedFreeDataArrayHelper(const IMarshalTargetInternal& atargetMarshaller)
		:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), nestedMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			// Release the allocated data arrays
			Internal::DeleteSTLContainerItemArray(itemArray, (ContainerType*)0);
			delete[] elementSizeArray;
			if (Internal::nested_container_has_keys<ContainerType>::value)
			{
				Internal::DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, 0, (ContainerType*)0);
				delete[] nestedMarshallerArray;
			}
		}

	public:
		const IMarshalTargetInternal& targetMarshaller;
		void* itemArray;
		size_t* elementSizeArray;
		Internal::INestedMarshallerBase** nestedMarshallerArray;
	};
};
#endif
} // Close namespace Internal

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
class IMarshalTarget
{ };

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType, bool IsThisOrNextElementLastElement, bool IsOnlyMovable>
class IMarshalTarget<ContainerType, false, IsThisOrNextElementLastElement, IsOnlyMovable> :public Internal::IMarshalTargetInternal<ContainerType, IsOnlyMovable>
{ };

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalTarget<ContainerType, true, true, false>
{
public:
	// Marshalling methods
	inline void MarshalFrom(const ContainerType& sourceObject) const
	{
		MarshalDirectFromSource(sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type), sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void MarshalFrom(ContainerType&& sourceObject) const
	{
		MarshalDirectFromSource(sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type), std::move(sourceObject));
	}
#endif

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalDirectFromSource(size_t elementByteSize, const ContainerType& sourceObject) const = 0;
	// Note that although this virtual function is only required if C++11 support exists in order to perform a move, we
	// still need to provide it regardless of the level of C++ support for the current compiler, as marshalling may be
	// occurring between two assemblies, one of which has been compiled with a C++11 compliant compiler, and the other
	// which has not. This object needs to have the same virtual function table in both modules in order to allow them to
	// communicate. This same rule applies for other such occurrences in other IMarshalTarget specializations.
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalDirectFromSource(size_t elementByteSize, ContainerType& sourceObject) const = 0;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalTarget<ContainerType, true, true, true>
{
public:
	// Marshalling methods
	inline void MarshalFrom(ContainerType&& sourceObject) const
	{
		MarshalDirectFromSource(sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type), std::move(sourceObject));
	}

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalDirectFromSource(size_t elementByteSize, ContainerType&& sourceObject) const = 0;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class IMarshalTarget<std::vector<ElementType, Alloc>, false, true, false>
{
public:
	// Marshalling methods
	inline void MarshalFrom(const std::vector<ElementType, Alloc>& sourceObject) const
	{
		MarshalFromInternal(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void MarshalFrom(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		MarshalFromInternal(std::move(sourceObject));
	}
#endif

	// Capacity methods
	inline void resize(typename std::vector<ElementType, Alloc>::size_type newSize, const ElementType& insertElement = ElementType()) const
	{
		SetSize(static_cast<size_t>(newSize), insertElement);
	}
	inline void reserve(typename std::vector<ElementType, Alloc>::size_type newSize) const
	{
		SetReservedSize(static_cast<size_t>(newSize));
	}

	// Element access methods
	inline void SetElement(typename std::vector<ElementType, Alloc>::size_type index, const ElementType& element) const
	{
		MarshalSetElement(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), static_cast<size_t>(index), &element);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void SetElement(typename std::vector<ElementType, Alloc>::size_type index, ElementType&& element) const
	{
		MarshalSetElement(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), static_cast<size_t>(index), &element);
	}
#endif

	// Modifier methods
	inline void push_back(const ElementType& element) const
	{
		MarshalPushBack(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), &element);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void push_back(ElementType&& element) const
	{
		MarshalPushBack(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), &element);
	}
#endif
	inline void pop_back() const
	{
		PopBack();
	}
	inline void clear() const
	{
		Clear();
	}
	inline void assign(typename std::vector<ElementType, Alloc>::size_type newSize, const ElementType& element) const
	{
		Clear();
		SetSize(static_cast<size_t>(newSize), element);
	}
	inline void AssignFrom(const ElementType* data, size_t count) const
	{
		MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), data, count);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void AssignFromWithMove(ElementType* data, size_t count) const
	{
		MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), data, count);
	}
#endif

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalPushBack(size_t elementByteSize, const ElementType* sourceData) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalPushBack(size_t elementByteSize, ElementType* sourceData) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const = 0;

	// Size methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION Clear() const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION PopBack() const = 0;
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION SetReservedSize(size_t newSize) const = 0;
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION SetSize(size_t newSize, const ElementType& insertElement) const = 0;

private:
	// Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		// Obtain a pointer to the first data element in the source object
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		const ElementType* sourceData = sourceObject.data();
#else
		const ElementType* sourceData = sourceObject.empty()? 0: &sourceObject[0];
#endif

		// Marshal the contents of this container directly into the target object. We move the elements if the supplied
		// source is a non-const rvalue reference, otherwise we copy the elements.
		if (Internal::is_non_const_rvalue_reference<MARSHALSUPPORT_UNIVERSALREFERENCE(FullType)>::value)
		{
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), const_cast<ElementType*>(sourceData), sourceObject.size());
		}
		else
		{
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), sourceData, sourceObject.size());
		}
	}
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class IMarshalTarget<std::vector<ElementType, Alloc>, false, true, true>
{
public:
	// Marshalling methods
	inline void MarshalFrom(std::vector<ElementType, Alloc>&& sourceObject) const
	{
		// Obtain a pointer to the first data element in the source object
		ElementType* sourceData = sourceObject.data();

		// Marshal the contents of this container directly into the target object
		MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), sourceData, sourceObject.size());
	}

	// Capacity methods
	inline void reserve(typename std::vector<ElementType, Alloc>::size_type newSize) const
	{
		SetReservedSize(static_cast<size_t>(newSize));
	}

	// Element access methods
	inline void SetElement(typename std::vector<ElementType, Alloc>::size_type index, ElementType&& element) const
	{
		MarshalSetElement(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), static_cast<size_t>(index), &element);
	}

	// Modifier methods
	inline void push_back(ElementType&& element) const
	{
		MarshalPushBack(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), &element);
	}
	inline void pop_back() const
	{
		PopBack();
	}
	inline void clear() const
	{
		Clear();
	}
	inline void AssignFromWithMove(ElementType* data, size_t count) const
	{
		MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), data, count);
	}

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalPushBack(size_t elementByteSize, ElementType* sourceData) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const = 0;

	// Size methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION Clear() const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION PopBack() const = 0;
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION SetReservedSize(size_t newSize) const = 0;
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const = 0;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class Alloc>
class IMarshalTarget<std::vector<bool, Alloc>, false, true, false>
{
public:
	// Marshalling methods
	void MarshalFrom(const std::vector<bool, Alloc>& sourceObject) const
	{
		// Since the C++ standard defines a rather poor specialization of std::vector for the bool type which bitpacks
		// values, and therefore can't provide a reference to its underlying data members like the vector container
		// normally can, we need to repack the elements of this boolean vector here into a flat array so that we can
		// marshal it.
		size_t sourceObjectLength = sourceObject.size();
		bool* itemArray = new bool[sourceObjectLength];
		for (size_t i = 0; i < sourceObjectLength; ++i)
		{
			itemArray[i] = sourceObject[i];
		}

		// Marshal the vector data to the target object
		MarshalFrom(itemArray, sourceObjectLength);

		// Delete the temporary array of unpacked vector data
		delete[] itemArray;
	}

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(const bool* sourceData, size_t sourceDataLength) const = 0;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class IMarshalTarget<std::array<ElementType, ArraySize>, false, true, false>
{
public:
	// Marshalling methods
	inline void MarshalFrom(const std::array<ElementType, ArraySize>& sourceObject) const
	{
		MarshalFromInternal(sourceObject);
	}
	inline void MarshalFrom(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		MarshalFromInternal(std::move(sourceObject));
	}

	// Element access methods
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, const ElementType& element) const
	{
		MarshalSetElement(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), static_cast<size_t>(index), &element);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType&& element) const
	{
		MarshalSetElement(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), static_cast<size_t>(index), &element);
	}
#endif

	// Modifier methods
	inline void AssignFrom(const ElementType* data) const
	{
		MarshalRange(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), 0, data, ArraySize);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void AssignFromWithMove(ElementType* data, size_t count) const
	{
		MarshalRange(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), 0, data, ArraySize);
	}
#endif

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, const ElementType* sourceData) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const = 0;

private:
	// Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		// Obtain a pointer to the first data element in the source object
		const ElementType* sourceData = sourceObject.data();

		// Marshal the contents of this container directly into the target object. We move the elements if the supplied
		// source is a non-const rvalue reference, otherwise we copy the elements.
		if (Internal::is_non_const_rvalue_reference<MARSHALSUPPORT_UNIVERSALREFERENCE(FullType)>::value)
		{
			MarshalRange(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), 0, const_cast<ElementType*>(sourceData), ArraySize);
		}
		else
		{
			MarshalRange(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), 0, sourceData, ArraySize);
		}
	}
};

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class IMarshalTarget<std::array<ElementType, ArraySize>, false, true, true>
{
public:
	// Marshalling methods
	inline void MarshalFrom(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		// Obtain a pointer to the first data element in the source object
		ElementType* sourceData = sourceObject.data();

		// Marshal the contents of this container directly into the target object
		MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), 0, sourceData, ArraySize);
	}

	// Element access methods
	inline void SetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType&& element) const
	{
		MarshalSetElement(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), static_cast<size_t>(index), &element);
	}

	// Modifier methods
	inline void AssignFromWithMove(ElementType* data) const
	{
		MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), 0, data, ArraySize);
	}

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalRange(size_t elementByteSize, size_t elementIndex, ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalSetElement(size_t elementByteSize, size_t elementIndex, ElementType* sourceData) const = 0;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class IMarshalTarget<std::basic_string<ElementType, traits, Alloc>, false, true, false>
{
public:
	// Marshalling methods
	inline void MarshalFrom(const std::basic_string<ElementType, traits, Alloc>& sourceObject) const
	{
		MarshalFromInternal(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void MarshalFrom(std::basic_string<ElementType, traits, Alloc>&& sourceObject) const
	{
		MarshalFromInternal(std::move(sourceObject));
	}
#endif

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const = 0;

private:
	// Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		// Obtain a pointer to the first data element in the source object
		const ElementType* sourceData = sourceObject.data();

		// Marshal the contents of this container directly into the target object. We move the elements if the supplied
		// source is a non-const rvalue reference, otherwise we copy the elements.
		if (Internal::is_non_const_rvalue_reference<MARSHALSUPPORT_UNIVERSALREFERENCE(FullType)>::value)
		{
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), const_cast<ElementType*>(sourceData), sourceObject.size());
		}
		else
		{
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), sourceData, sourceObject.size());
		}
	}
};

//----------------------------------------------------------------------------------------------------------------------
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class T, class Deleter>
class IMarshalTarget<std::unique_ptr<T, Deleter>, false, true, false>
{
public:
	// Marshalling methods
	inline void MarshalFrom(std::unique_ptr<T, Deleter>&& sourceObject) const
	{
		MarshalFrom(sourceObject.release(), sourceObject.get_deleter());
	}

protected:
	// Constructors
	inline ~IMarshalTarget() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION MarshalFrom(T* data, const Deleter& deleter) const = 0;
};
#endif

// Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // Close namespace MarshalSupport
#endif
