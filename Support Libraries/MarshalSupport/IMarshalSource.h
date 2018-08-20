#ifndef __IMARSHALSOURCE_H__
#define __IMARSHALSOURCE_H__
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
#include "INestedMarshaller.h"
#include <vector>
#include <string>
#include <utility>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#include <type_traits>
#include <tuple>
#endif
#include <cassert>
namespace MarshalSupport {
// Forward declare our Marshal::Ret class
namespace Marshal { template<class, bool> class Ret; }

#ifdef _MSC_VER
#pragma warning(push)
// Disable warning about conditional expressions being constant. The code behaves as intended, and the compiler is free
// to optimize away any dead code branches.
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
// This is the actual definition of the IMarshalSourceInternal class template. Note however that this is never used
// directly. We provide specializations of this template that cover all cases.
template<class ContainerType, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value, bool IsAssignable = Internal::is_assignable<ContainerType>::value>
class IMarshalSourceInternal
{ };

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalSourceInternal<ContainerType, false, true>
{
public:
	// Marshalling methods
	inline ContainerType MarshalTo() const
	{
		return MarshalToInternal<true>();
	}
	inline void MarshalTo(ContainerType& targetObject) const
	{
		MarshalToInternal<true>(targetObject);
	}
	inline ContainerType MarshalToWithoutMove() const
	{
		return MarshalToInternal<false>();
	}
	inline void MarshalToWithoutMove(ContainerType& targetObject) const
	{
		MarshalToInternal<false>(targetObject);
	}

protected:
	// Constructors
	inline ~IMarshalSourceInternal() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::INestedMarshallerBase* const*& nestedMarshallerArray, bool allowMove) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const = 0;

private:
	// Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		// Constructors
		ScopedFreeDataArrayHelper(const IMarshalSourceInternal& atargetMarshaller)
		:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), nestedMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			// Release the allocated data arrays
			targetMarshaller.FreeDataArrays(itemArray, elementSizeArray, nestedMarshallerArray);
		}

	public:
		const IMarshalSourceInternal& targetMarshaller;
		void* itemArray;
		const size_t* elementSizeArray;
		Internal::INestedMarshallerBase* const* nestedMarshallerArray;
	};
#ifndef MARSHALSUPPORT_CPP11SUPPORTED
	// This friend declaration is not required in C++11, but is added for earlier versions of the C++ standard due to
	// ambiguity and contradiction in prior versions regarding nested class access to its enclosing class.
	friend class ScopedFreeDataArrayHelper;
#endif

private:
	// Marshalling methods
	template<bool AllowMove>
	ContainerType MarshalToInternal() const
	{
		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, AllowMove);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);

		// Recompose our flat arrays into a new object
		ContainerType targetObject;
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);

		// Return the marshalled object to the caller
		return targetObject;
	}
	template<bool AllowMove>
	void MarshalToInternal(ContainerType& targetObject) const
	{
		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, AllowMove);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);

		// Ensure the target container is cleared of any current data
		Internal::ClearSTLContainer(targetObject);

		// Recompose our flat arrays into the target object
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}
};

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalSourceInternal<ContainerType, false, false>
{
public:
	// Marshalling methods
	inline ContainerType MarshalTo() const
	{
		return MarshalToInternal<true>();
	}
	inline ContainerType MarshalToWithoutMove() const
	{
		return MarshalToInternal<false>();
	}

protected:
	// Constructors
	inline ~IMarshalSourceInternal() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::INestedMarshallerBase* const*& nestedMarshallerArray, bool allowMove) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const = 0;

private:
	// Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		// Constructors
		ScopedFreeDataArrayHelper(const IMarshalSourceInternal& atargetMarshaller)
		:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), nestedMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			// Release the allocated data arrays
			targetMarshaller.FreeDataArrays(itemArray, elementSizeArray, nestedMarshallerArray);
		}

	public:
		const IMarshalSourceInternal& targetMarshaller;
		void* itemArray;
		const size_t* elementSizeArray;
		Internal::INestedMarshallerBase* const* nestedMarshallerArray;
	};
#ifndef MARSHALSUPPORT_CPP11SUPPORTED
	// This friend declaration is not required in C++11, but is added for earlier versions of the C++ standard due to
	// ambiguity and contradiction in prior versions regarding nested class access to its enclosing class.
	friend class ScopedFreeDataArrayHelper;
#endif

private:
	// Marshalling methods
	template<bool AllowMove>
	ContainerType MarshalToInternal() const
	{
		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, AllowMove);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);

		// Recompose our flat arrays into a new object
		ContainerType targetObject;
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);

		// Return the marshalled object to the caller
		return targetObject;
	}
	template<bool AllowMove>
	void MarshalToInternal(ContainerType& targetObject) const
	{
		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, AllowMove);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);

		// Ensure the target container is cleared of any current data
		Internal::ClearSTLContainer(targetObject);

		// Recompose our flat arrays into the target object
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalSourceInternal<ContainerType, true, true>
{
public:
	// Marshalling methods
	inline ContainerType MarshalTo() const
	{
		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);

		// Recompose our flat arrays into a new object
		ContainerType targetObject;
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);

		// Return the marshalled object to the caller
		return targetObject;
	}
	inline void MarshalTo(ContainerType& targetObject) const
	{
		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, AllowMove);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);

		// Ensure the target container is cleared of any current data
		Internal::ClearSTLContainer(targetObject);

		// Recompose our flat arrays into the target object
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}

protected:
	// Constructors
	inline ~IMarshalSourceInternal() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::INestedMarshallerBase* const*& nestedMarshallerArray) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const = 0;

private:
	// Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		// Constructors
		ScopedFreeDataArrayHelper(const IMarshalSourceInternal& atargetMarshaller)
		:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), nestedMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			// Release the allocated data arrays
			targetMarshaller.FreeDataArrays(itemArray, elementSizeArray, nestedMarshallerArray);
		}

	public:
		const IMarshalSourceInternal& targetMarshaller;
		void* itemArray;
		const size_t* elementSizeArray;
		Internal::INestedMarshallerBase* const* nestedMarshallerArray;
	};
};

//----------------------------------------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalSourceInternal<ContainerType, true, false>
{
public:
	// Marshalling methods
	inline ContainerType MarshalTo() const
	{
		// Retrieve arrays to load the decomposed object data from. Note that we give ownership of the arrays to a helper
		// object which frees the data in its destructor, to ensure we don't leak memory if an exception is triggered
		// during the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Ensure our key marshallers agree on the marshalled object size
		Internal::EnsureSTLContainerKeySizesMatch(arrayHelper.nestedMarshallerArray, 0, (ContainerType*)0);

		// Recompose our flat arrays into a new object
		ContainerType targetObject;
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);

		// Return the marshalled object to the caller
		return targetObject;
	}

protected:
	// Constructors
	inline ~IMarshalSourceInternal() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::INestedMarshallerBase* const*& nestedMarshallerArray) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::INestedMarshallerBase* const* nestedMarshallerArray) const = 0;

private:
	// Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		// Constructors
		ScopedFreeDataArrayHelper(const IMarshalSourceInternal& atargetMarshaller)
			:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), nestedMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			// Release the allocated data arrays
			targetMarshaller.FreeDataArrays(itemArray, elementSizeArray, nestedMarshallerArray);
		}

	public:
		const IMarshalSourceInternal& targetMarshaller;
		void* itemArray;
		const size_t* elementSizeArray;
		Internal::INestedMarshallerBase* const* nestedMarshallerArray;
	};
};
#endif
} // Close namespace Internal

//----------------------------------------------------------------------------------------------------------------------
// This is the actual definition of the IMarshalSource class template. Note however that this is never used directly. We
// provide specializations of this template that cover all cases.
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value, bool IsOnlyMovable = Internal::is_only_movable<typename Internal::get_last_nested_container_element_type<ContainerType>::type>::value>
class IMarshalSource
{ };

//----------------------------------------------------------------------------------------------------------------------
// Specialize IMarshalSource for any case where the target object isn't directly assignable or marshallable. By itself
// this specialization will cover all use of STL containers, including containers nested within other containers, however
// there are some further specializations below to optimize marshalling performance for various STL container types where
// the contained elements are directly assignable or marshallable. Also note we don't actually do any direct
// implementation here, rather we pull in the implementation from a templated base type. This allows us to inject more
// template parameters and further specialize in this base class, without cluttering up the IMarshalSource template with
// more parameters.
template<class ContainerType, bool IsThisOrNextElementLastElement, bool IsOnlyMovable>
class IMarshalSource<ContainerType, false, IsThisOrNextElementLastElement, IsOnlyMovable> :public Internal::IMarshalSourceInternal<ContainerType, IsOnlyMovable>
{ };

//----------------------------------------------------------------------------------------------------------------------
// Specialize IMarshalSource for the case that the source object is directly assignable or marshallable, and where a move
// operation is not strictly required.
template<class ContainerType>
class IMarshalSource<ContainerType, true, true, false>
{
public:
	// Marshalling methods
	inline ContainerType MarshalTo() const
	{
		return MarshalToInternal<true>();
	}
	inline void MarshalTo(ContainerType& targetObject) const
	{
		MarshalToInternal<true>(targetObject);
	}
	inline ContainerType MarshalToWithoutMove() const
	{
		return MarshalToInternal<false>();
	}
	inline void MarshalToWithoutMove(ContainerType& targetObject) const
	{
		MarshalToInternal<false>(targetObject);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ContainerType*& sourceData) const = 0;

private:
	// Marshalling methods
	template<bool AllowMove>
	ContainerType MarshalToInternal() const
	{
		// Retrieve a reference to the object being marshalled
		size_t elementByteSize;
		const ContainerType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Marshal the object, performing a move, copy, or invoking a marshal constructor as required.
		if (sourceDataMovable && AllowMove)
		{
			return Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToNewObject(MARSHALSUPPORT_MOVE(const_cast<ContainerType&>(*sourceData)));
		}
		else
		{
			return Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToNewObject(*sourceData);
		}
	}
	template<bool AllowMove>
	void MarshalToInternal(ContainerType& targetObject) const
	{
		// Retrieve a reference to the object being marshalled
		size_t elementByteSize;
		const ContainerType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Marshal the object, performing a move, copy, or invoking a marshal constructor as required.
		if (sourceDataMovable && AllowMove)
		{
			Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(const_cast<ContainerType&>(*sourceData)), targetObject);
		}
		else
		{
			Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(*sourceData, targetObject);
		}
	}
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
// Specialize IMarshalSource for the case that the source object is directly assignable or marshallable, but where the
// object can only be moved, not copied.
template<class ContainerType>
class IMarshalSource<ContainerType, true, true, true>
{
public:
	// Marshalling methods
	inline ContainerType MarshalTo() const
	{
		// Retrieve a reference to the object being marshalled
		size_t elementByteSize;
		ContainerType* sourceData;
		RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Marshal the object, performing a move, copy, or invoking a marshal constructor as required.
		return Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToNewObject(std::move(*sourceData));
	}
	inline void MarshalTo(ContainerType& targetObject) const
	{
		// Retrieve a reference to the object being marshalled
		size_t elementByteSize;
		ContainerType* sourceData;
		RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		// Marshal the object, performing a move, copy, or invoking a marshal constructor as required.
		Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(std::move(*sourceData), targetObject);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, ContainerType*& sourceData) const = 0;
};
#endif

//----------------------------------------------------------------------------------------------------------------------
// Specialize IMarshalSource for a vector containing an element that is directly assignable or marshallable, and where a
// move operation is not strictly required. Note that this specialization isn't just for performance, it also provides
// some additional methods that can allow the vector object to be interacted with and vector elements to be marshalled
// individually.
template<class ElementType, class Alloc>
class IMarshalSource<std::vector<ElementType, Alloc>, false, true, false>
{
public:
	// Marshalling methods
	inline std::vector<ElementType, Alloc> MarshalTo() const
	{
		std::vector<ElementType, Alloc> result;
		MarshalToInternal<true>(result);
		return result;
	}
	inline void MarshalTo(std::vector<ElementType, Alloc>& targetObject) const
	{
		MarshalToInternal<true>(targetObject);
	}
	inline std::vector<ElementType, Alloc> MarshalToWithoutMove() const
	{
		std::vector<ElementType, Alloc> result;
		MarshalToInternal<false>(result);
		return result;
	}
	inline void MarshalToWithoutMove(std::vector<ElementType, Alloc>& targetObject) const
	{
		MarshalToInternal<false>(targetObject);
	}

	// Capacity methods
	inline typename std::vector<ElementType, Alloc>::size_type size() const
	{
		return static_cast<typename std::vector<ElementType, Alloc>::size_type>(GetSize());
	}
	inline typename std::vector<ElementType, Alloc>::size_type capacity() const
	{
		return static_cast<typename std::vector<ElementType, Alloc>::size_type>(GetReservedSize());
	}
	inline bool empty() const
	{
		return (GetSize() == 0);
	}

	// Element access methods
	inline void GetElement(typename std::vector<ElementType, Alloc>::size_type index, ElementType& element) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		size_t sourceDataLength;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData, sourceDataLength);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// Marshal the object. Note that since the object has a custom marshal constructor, it's possible the size and
		// layout of the object differs on each side of the boundary. We've received a pointer to an array of source
		// objects to marshal, but we can't index that array using our size of the object here, since it may differ from
		// that of the sender. To account for this, we calculate the address of each object in the array using the object
		// byte size specified by the sender.
		const ElementType* sourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + (elementByteSize * static_cast<size_t>(index)));
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*sourceObject, element);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ElementType*& sourceData, size_t& sourceDataLength) const = 0;

	// Size methods
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const = 0;
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const = 0;

private:
	// Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::vector<ElementType, Alloc>& targetObject) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		size_t sourceDataLength;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData, sourceDataLength);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// If the target element type has a custom marshaller, marshal the elements one by one, otherwise copy the
		// contents of the source container directly into the target object.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			targetObject.clear();
			targetObject.reserve(sourceDataLength);

			// Marshal the object. Note that since the object has a custom marshal constructor, it's possible the size
			// and layout of the object differs on each side of the boundary. We've received a pointer to an array of
			// source objects to marshal, but we can't index that array using our size of the object here, since it may
			// differ from that of the sender. To account for this, we calculate the address of each object in the array
			// using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			if (sourceDataMovable && AllowMove)
			{
				for (size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(const_cast<ElementType&>(*nextSourceObject)), targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
			else
			{
				for (size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(*nextSourceObject, targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data, performing a move operation if one
			// is available and permitted. Note that in addition, we only use move iterators where the target elements
			// use a non-trivial type. This is done as an optimization. Although the assign method in theory has all the
			// necessary information to optimize copying an array of standard layout types in one process using memcpy,
			// in VS2013 it was observed that the move iterator wrappers prevent this optimization occurring. We avoid
			// using move iterators in this case to achieve maximum efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if (!std::is_trivial<ElementType>::value && sourceDataMovable && AllowMove)
			{
				targetObject.assign(std::make_move_iterator(const_cast<ElementType*>(sourceData)), std::make_move_iterator(const_cast<ElementType*>(sourceData) + sourceDataLength));
			}
			else
#endif
			{
				targetObject.assign(sourceData, sourceData + sourceDataLength);
			}
		}
	}
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
// Specialize IMarshalSource for a vector containing an element that is directly assignable or marshallable, but where
// the object can only be moved, not copied.
template<class ElementType, class Alloc>
class IMarshalSource<std::vector<ElementType, Alloc>, false, true, true>
{
public:
	// Marshalling methods
	inline std::vector<ElementType, Alloc> MarshalTo() const
	{
		std::vector<ElementType, Alloc> result;
		MarshalToInternal(result);
		return result;
	}
	inline void MarshalTo(std::vector<ElementType, Alloc>& targetObject) const
	{
		MarshalToInternal(targetObject);
	}

	// Capacity methods
	inline typename std::vector<ElementType, Alloc>::size_type size() const
	{
		return static_cast<typename std::vector<ElementType, Alloc>::size_type>(GetSize());
	}
	inline typename std::vector<ElementType, Alloc>::size_type capacity() const
	{
		return static_cast<typename std::vector<ElementType, Alloc>::size_type>(GetReservedSize());
	}
	inline bool empty() const
	{
		return (GetSize() == 0);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, ElementType*& sourceData, size_t& sourceDataLength) const = 0;

	// Size methods
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetSize() const = 0;
	virtual size_t MARSHALSUPPORT_CALLINGCONVENTION GetReservedSize() const = 0;

private:
	// Marshalling methods
	void MarshalToInternal(std::vector<ElementType, Alloc>& targetObject) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		ElementType* sourceData;
		size_t sourceDataLength;
		RetrieveData(elementByteSize, sourceData, sourceDataLength);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		// If the target element type has a custom marshaller, marshal the elements one by one, otherwise copy the
		// contents of the source container directly into the target object.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			targetObject.clear();
			targetObject.reserve(sourceDataLength);

			// Marshal the object. Note that since the object has a custom marshal constructor, it's possible the size
			// and layout of the object differs on each side of the boundary. We've received a pointer to an array of
			// source objects to marshal, but we can't index that array using our size of the object here, since it may
			// differ from that of the sender. To account for this, we calculate the address of each object in the array
			// using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for (size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(std::move(*nextSourceObject), targetObject);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data
			targetObject.assign(std::make_move_iterator(sourceData), std::make_move_iterator(sourceData + sourceDataLength));
		}
	}
};
#endif

//----------------------------------------------------------------------------------------------------------------------
// Specialize IMarshalSource for a vector of bool values. We need this specialization to handle the rather unfortunate
// vector<bool> specialization that still exists in the C++ language, which isn't compatible with standard STL
// containers.
template<class Alloc>
class IMarshalSource<std::vector<bool, Alloc>, false, true, false>
{
public:
	// Marshalling methods
	inline std::vector<bool, Alloc> MarshalTo() const
	{
		std::vector<bool, Alloc> result;
		MarshalToInternal<true>(result);
		return result;
	}
	inline void MarshalTo(std::vector<bool, Alloc>& targetObject) const
	{
		MarshalToInternal<true>(targetObject);
	}
	inline std::vector<bool, Alloc> MarshalToWithoutMove() const
	{
		std::vector<bool, Alloc> result;
		MarshalToInternal<false>(result);
		return result;
	}
	inline void MarshalToWithoutMove(std::vector<bool, Alloc>& targetObject) const
	{
		MarshalToInternal<false>(targetObject);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(const bool*& sourceData, size_t& sourceDataLength) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION FreeDataArrays(const bool* sourceData) const = 0;

private:
	// Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::vector<bool, Alloc>& targetObject) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		const bool* sourceData;
		size_t sourceDataLength;
		RetrieveData(sourceData, sourceDataLength);

		// Copy the contents of the source container directly into the target object
		targetObject.assign(sourceData, sourceData + sourceDataLength);

		// Release the allocated data arrays
		FreeDataArrays(sourceData);
	}
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class IMarshalSource<std::array<ElementType, ArraySize>, false, true, false>
{
public:
	// Marshalling methods
	inline std::array<ElementType, ArraySize> MarshalTo() const
	{
		std::array<ElementType, ArraySize> result;
		MarshalToInternal<true>(result);
		return result;
	}
	inline void MarshalTo(std::array<ElementType, ArraySize>& targetObject) const
	{
		MarshalToInternal<true>(targetObject);
	}
	inline std::array<ElementType, ArraySize> MarshalToWithoutMove() const
	{
		std::array<ElementType, ArraySize> result;
		MarshalToInternal<false>(result);
		return result;
	}
	inline void MarshalToWithoutMove(std::array<ElementType, ArraySize>& targetObject) const
	{
		MarshalToInternal<false>(targetObject);
	}

	// Capacity methods
	inline typename std::array<ElementType, ArraySize>::size_type size() const
	{
		return ArraySize;
	}
	inline bool empty() const
	{
		return (ArraySize() == 0);
	}

	// Element access methods
	inline void GetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType& element) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Marshal the object. Note that if the object has a custom marshal constructor, it's possible the size and
		// layout of the object differs on each side of the boundary. We've received a pointer to an array of source
		// objects to marshal, but we can't index that array using our size of the object here, since it may differ from
		// that of the sender. To account for this, we calculate the address of each object in the array using the object
		// byte size specified by the sender.
		const ElementType* sourceElement = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(sourceData) + (elementByteSize * index));
		if (sourceDataMovable && AllowMove)
		{
			Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(const_cast<ElementType&>(*sourceElement)), element);
		}
		else
		{
			Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*sourceElement, element);
		}
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ElementType*& sourceData) const = 0;

private:
	// Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::array<ElementType, ArraySize>& targetObject) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// If the target element type has a custom marshaller, marshal the elements one by one, otherwise copy the
		// contents of the source container directly into the target object.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal the object. Note that since the object has a custom marshal constructor, it's possible the size
			// and layout of the object differs on each side of the boundary. We've received a pointer to an array of
			// source objects to marshal, but we can't index that array using our size of the object here, since it may
			// differ from that of the sender. To account for this, we calculate the address of each object in the array
			// using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			if (sourceDataMovable && AllowMove)
			{
				for (size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(const_cast<ElementType&>(*nextSourceObject)), targetObject[(typename std::array<ElementType, ArraySize>::size_type)i]);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
			else
			{
				for (size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*nextSourceObject, targetObject[(typename std::array<ElementType, ArraySize>::size_type)i]);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
		}
		else
		{
			// Directly initialize the target array with the supplied array of data, performing a direct memory copy if
			// safe, or performing a move operation if one is available and permitted.
			if (std::is_trivial<ElementType>::value)
			{
				memcpy((void*)targetObject.data(), (const void*)sourceData, ArraySize * sizeof(ElementType));
			}
			else if (sourceDataMovable && AllowMove)
			{
				for (size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(const_cast<ElementType&>(sourceData[i])), targetObject[(typename std::array<ElementType, ArraySize>::size_type)i]);
				}
			}
			else
			{
				for (size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(sourceData[i], targetObject[(typename std::array<ElementType, ArraySize>::size_type)i]);
				}
			}
		}
	}
};

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class IMarshalSource<std::array<ElementType, ArraySize>, false, true, true>
{
public:
	// Marshalling methods
	inline std::array<ElementType, ArraySize> MarshalTo() const
	{
		std::array<ElementType, ArraySize> result;
		MarshalToInternal<true>(result);
		return result;
	}
	inline void MarshalTo(std::array<ElementType, ArraySize>& targetObject) const
	{
		MarshalToInternal<true>(targetObject);
	}

	// Capacity methods
	inline typename std::array<ElementType, ArraySize>::size_type size() const
	{
		return ArraySize;
	}
	inline bool empty() const
	{
		return (ArraySize() == 0);
	}

	// Element access methods
	inline void GetElement(typename std::array<ElementType, ArraySize>::size_type index, ElementType& element) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// Marshal the object. Note that if the object has a custom marshal constructor, it's possible the size and
		// layout of the object differs on each side of the boundary. We've received a pointer to an array of source
		// objects to marshal, but we can't index that array using our size of the object here, since it may differ from
		// that of the sender. To account for this, we calculate the address of each object in the array using the object
		// byte size specified by the sender.
		ElementType* sourceElement = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(sourceData) + (elementByteSize * index));
		Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*sourceElement), targetObject[(typename std::array<ElementType, ArraySize>::size_type)i]);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ElementType*& sourceData) const = 0;

private:
	// Marshalling methods
	void MarshalToInternal(std::array<ElementType, ArraySize>& targetObject) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		ElementType* sourceData;
		RetrieveData(elementByteSize, sourceData);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		// If the target element type has a custom marshaller, marshal the elements one by one, otherwise copy the
		// contents of the source container directly into the target object.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Marshal the object. Note that since the object has a custom marshal constructor, it's possible the size
			// and layout of the object differs on each side of the boundary. We've received a pointer to an array of
			// source objects to marshal, but we can't index that array using our size of the object here, since it may
			// differ from that of the sender. To account for this, we calculate the address of each object in the array
			// using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for (size_t i = 0; i < ArraySize; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*nextSourceObject), targetObject[(typename std::array<ElementType, ArraySize>::size_type)i]);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			// Directly initialize the target array with the supplied array of data
			for (size_t i = 0; i < ArraySize; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(sourceData[i]), targetObject[(typename std::array<ElementType, ArraySize>::size_type)i]);
			}
		}
	}
};
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class IMarshalSource<std::basic_string<ElementType, traits, Alloc>, false, true, false>
{
public:
	// Marshalling methods
	inline std::basic_string<ElementType, traits, Alloc> MarshalTo() const
	{
		std::basic_string<ElementType, traits, Alloc> result;
		MarshalToInternal<true>(result);
		return result;
	}
	inline void MarshalTo(std::basic_string<ElementType, traits, Alloc>& targetObject) const
	{
		MarshalToInternal<true>(targetObject);
	}
	inline std::basic_string<ElementType, traits, Alloc> MarshalToWithoutMove() const
	{
		std::basic_string<ElementType, traits, Alloc> result;
		MarshalToInternal<false>(result);
		return result;
	}
	inline void MarshalToWithoutMove(std::basic_string<ElementType, traits, Alloc>& targetObject) const
	{
		MarshalToInternal<false>(targetObject);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual bool MARSHALSUPPORT_CALLINGCONVENTION RetrieveData(size_t& elementByteSize, const ElementType*& sourceData, size_t& sourceDataLength) const = 0;

private:
	// Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::basic_string<ElementType, traits, Alloc>& targetObject) const
	{
		// Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		size_t sourceDataLength;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData, sourceDataLength);

		// Ensure the size of the innermost element is the same as expected, or that a custom marshaller has been
		// supplied. The object being the correct size isn't a guarantee that marshalling is being used correctly, but if
		// the object size is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::basic_string<ElementType, traits, Alloc>*)0));

		// If the target element type has a custom marshaller, marshal the elements one by one, otherwise copy the
		// contents of the source container directly into the target object.
		if (Internal::has_marshal_constructor<ElementType>::value)
		{
			// Ensure the target vector is empty, with enough space reserved for the number of elements we're about to
			// insert in it.
			targetObject.clear();
			targetObject.reserve(sourceDataLength);

			// Marshal the object. Note that since the object has a custom marshal constructor, it's possible the size
			// and layout of the object differs on each side of the boundary. We've received a pointer to an array of
			// source objects to marshal, but we can't index that array using our size of the object here, since it may
			// differ from that of the sender. To account for this, we calculate the address of each object in the array
			// using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			if (sourceDataMovable && AllowMove)
			{
				for (size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(const_cast<ElementType&>(*nextSourceObject)), targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
			else
			{
				for (size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(*nextSourceObject, targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
		}
		else
		{
			// Directly initialize the target vector with the supplied array of data, performing a move operation if one
			// is available and permitted. Note that in addition, we only use move iterators where the target elements
			// use a non-trivial type. This is done as an optimization. Although the assign method in theory has all the
			// necessary information to optimize copying an array of standard layout types in one process using memcpy,
			// in VS2013 it was observed that the move iterator wrappers prevent this optimization occurring. We avoid
			// using move iterators in this case to achieve maximum efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if (!std::is_trivial<ElementType>::value && sourceDataMovable && AllowMove)
			{
				targetObject.assign(std::make_move_iterator(const_cast<ElementType*>(sourceData)), std::make_move_iterator(const_cast<ElementType*>(sourceData) + sourceDataLength));
			}
			else
#endif
			{
				targetObject.assign(sourceData, sourceData + sourceDataLength);
			}
		}
	}
};

//----------------------------------------------------------------------------------------------------------------------
template<class T1, class T2>
class IMarshalSource<std::pair<T1, T2>, false, true, false>
{
public:
	// Marshalling methods
	inline std::pair<T1, T2> MarshalTo() const
	{
		return std::pair<T1, T2>(RetrieveFirstElement(true).Get(), RetrieveSecondElement(true).Get());
	}
	inline void MarshalTo(std::pair<T1, T2>& targetObject) const
	{
		targetObject.first = RetrieveFirstElement(true).Get();
		targetObject.second = RetrieveSecondElement(true).Get();
	}
	inline std::pair<T1, T2> MarshalToWithoutMove() const
	{
		return std::pair<T1, T2>(RetrieveFirstElement(false).Get(), RetrieveSecondElement(false).Get());
	}
	inline void MarshalToWithoutMove(std::pair<T1, T2>& targetObject) const
	{
		targetObject.first = RetrieveFirstElement(false).Get();
		targetObject.second = RetrieveSecondElement(false).Get();
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual Marshal::Ret<T1, false> MARSHALSUPPORT_CALLINGCONVENTION RetrieveFirstElement(bool allowMove) const = 0;
	virtual Marshal::Ret<T2, false> MARSHALSUPPORT_CALLINGCONVENTION RetrieveSecondElement(bool allowMove) const = 0;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class T1, class T2>
class IMarshalSource<std::pair<T1, T2>, false, true, true>
{
public:
	// Marshalling methods
	inline std::pair<T1, T2> MarshalTo() const
	{
		return std::pair<T1, T2>(RetrieveFirstElement().Get(), RetrieveSecondElement().Get());
	}
	inline void MarshalTo(std::pair<T1, T2>& targetObject) const
	{
		targetObject.first = RetrieveFirstElement().Get();
		targetObject.second = RetrieveSecondElement().Get();
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual Marshal::Ret<T1, true> MARSHALSUPPORT_CALLINGCONVENTION RetrieveFirstElement() const = 0;
	virtual Marshal::Ret<T2, true> MARSHALSUPPORT_CALLINGCONVENTION RetrieveSecondElement() const = 0;
};
#endif

#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
//----------------------------------------------------------------------------------------------------------------------
template<>
class IMarshalSource<std::tuple<>, false, true, false>
{
public:
	// Marshalling methods
	inline std::tuple<> MarshalTo() const
	{
		return std::make_tuple();
	}
	inline void MarshalTo(std::tuple<>& targetObject) const
	{ }
	inline std::tuple<> MarshalToWithoutMove() const
	{
		return std::make_tuple();
	}
	inline void MarshalToWithoutMove(std::tuple<>& targetObject) const
	{ }

protected:
	// Constructors
	inline ~IMarshalSource() { }
};

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
class IMarshalSource<std::tuple<Args...>, false, true, false>
{
public:
	// Marshalling methods
	inline std::tuple<Args...> MarshalTo() const
	{
		Internal::INestedMarshallerBase* elementMarshallers[std::tuple_size<std::tuple<Args...>>::value];
		CreateElementMarshallers(elementMarshallers, true);
		std::tuple<Args...> result = MakeTupleFromElementMarshallers(typename GenerateIntegerSequence<sizeof...(Args)>::type(), elementMarshallers);
		DeleteElementMarshallers(elementMarshallers);
		return result;
	}
	inline void MarshalTo(std::tuple<Args...>& targetObject) const
	{
		Internal::INestedMarshallerBase* elementMarshallers[std::tuple_size<std::tuple<Args...>>::value];
		CreateElementMarshallers(elementMarshallers, true);
		std::tuple<Args...> result = MakeTupleFromElementMarshallers(typename GenerateIntegerSequence<sizeof...(Args)>::type(), elementMarshallers);
		targetObject = std::move(result);
		DeleteElementMarshallers(elementMarshallers);
	}
	inline std::tuple<Args...> MarshalToWithoutMove() const
	{
		Internal::INestedMarshallerBase* elementMarshallers[std::tuple_size<std::tuple<Args...>>::value];
		CreateElementMarshallers(elementMarshallers, false);
		std::tuple<Args...> result = MakeTupleFromElementMarshallers(typename GenerateIntegerSequence<sizeof...(Args)>::type(), elementMarshallers);
		DeleteElementMarshallers(elementMarshallers);
		return result;
	}
	inline void MarshalToWithoutMove(std::tuple<Args...>& targetObject) const
	{
		Internal::INestedMarshallerBase* elementMarshallers[std::tuple_size<std::tuple<Args...>>::value];
		CreateElementMarshallers(elementMarshallers, false);
		std::tuple<Args...> result = MakeTupleFromElementMarshallers(typename GenerateIntegerSequence<sizeof...(Args)>::type(), elementMarshallers);
		targetObject = std::move(result);
		DeleteElementMarshallers(elementMarshallers);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value], bool allowMove) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION DeleteElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value]) const = 0;

private:
	// Sequence generation
	template<int ...S>
	struct IntegerSequence { };
	template<int N, int ...S>
	struct GenerateIntegerSequence :GenerateIntegerSequence<N-1, N-1, S...> { };
	template<int ...S>
	struct GenerateIntegerSequence<0, S...> { typedef IntegerSequence<S...> type; };

private:
	// Marshalling methods
	template<int ...S>
	static std::tuple<Args...> MakeTupleFromElementMarshallers(IntegerSequence<S...>, Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value])
	{
		return std::make_tuple(((Internal::INestedMarshaller<typename std::tuple_element<S, std::tuple<Args...>>::type>*)elementMarshallers[S])->RemoveKey(sizeof(typename std::tuple_element<S, std::tuple<Args...>>::type)) ...);
	}
};

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
class IMarshalSource<std::tuple<Args...>, false, true, true>
{
public:
	// Marshalling methods
	inline std::tuple<Args...> MarshalTo() const
	{
		Internal::INestedMarshallerBase* elementMarshallers[std::tuple_size<std::tuple<Args...>>::value];
		CreateElementMarshallers(elementMarshallers);
		std::tuple<Args...> result = MakeTupleFromElementMarshallers(typename GenerateIntegerSequence<sizeof...(Args)>::type(), elementMarshallers);
		DeleteElementMarshallers(elementMarshallers);
		return result;
	}
	inline void MarshalTo(std::tuple<Args...>& targetObject) const
	{
		Internal::INestedMarshallerBase* elementMarshallers[std::tuple_size<std::tuple<Args...>>::value];
		CreateElementMarshallers(elementMarshallers);
		std::tuple<Args...> result = MakeTupleFromElementMarshallers(typename GenerateIntegerSequence<sizeof...(Args)>::type(), elementMarshallers);
		targetObject = std::move(result);
		DeleteElementMarshallers(elementMarshallers);
	}

protected:
	// Constructors
	inline ~IMarshalSource() { }

	// Marshalling methods
	virtual void MARSHALSUPPORT_CALLINGCONVENTION CreateElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value]) const = 0;
	virtual void MARSHALSUPPORT_CALLINGCONVENTION DeleteElementMarshallers(Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value]) const = 0;

private:
	// Sequence generation
	template<int ...S>
	struct IntegerSequence { };
	template<int N, int ...S>
	struct GenerateIntegerSequence :GenerateIntegerSequence<N-1, N-1, S...> { };
	template<int ...S>
	struct GenerateIntegerSequence<0, S...> { typedef IntegerSequence<S...> type; };

private:
	// Marshalling methods
	template<int ...S>
	static std::tuple<Args...> MakeTupleFromElementMarshallers(IntegerSequence<S...>, Internal::INestedMarshallerBase* (&elementMarshallers)[std::tuple_size<std::tuple<Args...>>::value])
	{
		return std::make_tuple(((Internal::INestedMarshaller<typename std::tuple_element<S, std::tuple<Args...>>::type>*)elementMarshallers[S])->RemoveKey(sizeof(typename std::tuple_element<S, std::tuple<Args...>>::type)) ...);
	}
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
