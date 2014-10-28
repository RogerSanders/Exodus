#ifndef __IMARSHALTARGET_H__
#define __IMARSHALTARGET_H__
#include "MarshalPreprocessorMacros.h"
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
#include "ISTLObjectKeyMarshallerBase.h"
#include <vector>
#include <string>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#endif
namespace MarshalSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about conditional expressions being constant. The code behaves as
//intended, and the compiler is free to optimize away any dead code branches.
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
namespace Internal {
template<class ContainerType>
class IMarshalTargetInternal
{
public:
	//Constructors
	inline virtual ~IMarshalTargetInternal() = 0;

	//Marshalling methods
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
	//Marshalling methods
	virtual void MarshalFromFlatArrays(size_t elementByteSize, void* itemArray, const size_t* elementSizeArray, Internal::ISTLObjectKeyMarshallerBase* const* keyMarshallerArray) const = 0;

private:
	//Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		//Constructors
		ScopedFreeDataArrayHelper(const IMarshalTargetInternal& atargetMarshaller)
		:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			//Release the allocated data arrays
			Internal::DeleteSTLContainerItemArray(itemArray, (ContainerType*)0);
			delete[] elementSizeArray;
			if(Internal::nested_container_has_keys<ContainerType>::value)
			{
				Internal::DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (ContainerType*)0);
				delete[] keyMarshallerArray;
			}
		}

	public:
		const IMarshalTargetInternal& targetMarshaller;
		void* itemArray;
		size_t* elementSizeArray;
		Internal::ISTLObjectKeyMarshallerBase** keyMarshallerArray;
	};
#ifndef MARSHALSUPPORT_CPP11SUPPORTED
	//This friend declaration is not required in C++11, but is added for earlier versions
	//of the C++ standard due to ambiguity and contradiction in prior versions regarding
	//nested class access to its enclosing class.
	friend class ScopedFreeDataArrayHelper;
#endif

private:
	//Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		//Calculate the required size of our flat arrays in order to hold a decomposed
		//version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		Internal::CalculateDecomposedSTLContainerSize(itemArraySize, elementSizeArraySize, sourceObject);

		//Retrieve arrays to load the decomposed object data from. Note that we give
		//ownership of the arrays to a helper object which frees the data in its
		//destructor, to ensure we don't leak memory if an exception is triggered during
		//the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		arrayHelper.itemArray = Internal::CreateSTLContainerItemArray(itemArraySize, &sourceObject);
		arrayHelper.elementSizeArray = new size_t[elementSizeArraySize];

		//Allocate key marshallers for any keyed containers in the source object
		if(Internal::nested_container_has_keys<ContainerType>::value)
		{
			MARSHALSUPPORT_CONSTEXPR size_t nestingDepth = Internal::CalculateSTLContainerNestingDepth(0, (ContainerType*)0);
			arrayHelper.keyMarshallerArray = new Internal::ISTLObjectKeyMarshallerBase*[nestingDepth];
			Internal::CreateSTLContainerKeyMarshallers(arrayHelper.keyMarshallerArray, 0, (ContainerType*)0);
		}

		//Decompose the container hierarchy into a flat array of the innermost elements,
		//with a corresponding array of sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t keyMarshallerArrayIndex = 0;
		Internal::DecomposeSTLContainer(arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, MARSHALSUPPORT_FORWARD(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType), sourceObject));

		//Recompose our flat arrays into the target container
		MarshalFromFlatArrays(sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type), arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.keyMarshallerArray);
	}
};
template<class ContainerType>
IMarshalTargetInternal<ContainerType>::~IMarshalTargetInternal() { }
} //Close namespace Internal

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class IMarshalTarget
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalTarget<ContainerType, false, false> :public Internal::IMarshalTargetInternal<ContainerType>
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalTarget<ContainerType, false, true> :public Internal::IMarshalTargetInternal<ContainerType>
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalTarget<ContainerType, true, true>
{
public:
	//Constructors
	inline virtual ~IMarshalTarget() = 0;

	//Marshalling methods
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
	//Marshalling methods
	virtual void MarshalDirectFromSource(size_t elementByteSize, const ContainerType& sourceObject) const = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void MarshalDirectFromSource(size_t elementByteSize, ContainerType&& sourceObject) const = 0;
#endif

private:
	//Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		//Since only a single object is being marshalled, marshal it directly to the
		//target.
		MarshalDirectFromSource(sizeof(typename Internal::get_last_nested_container_element_type<ContainerType>::type), MARSHALSUPPORT_FORWARD(FullType, sourceObject));
	}
};
template<class ContainerType>
IMarshalTarget<ContainerType, true, true>::~IMarshalTarget() { }

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class IMarshalTarget<std::vector<ElementType, Alloc>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalTarget() = 0;

	//Marshalling methods
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

protected:
	//Marshalling methods
	virtual void MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const = 0;
	//Note that although this virtual function is only required if C++11 support exists in
	//order to perform a move, we still need to provide it regardless of the level of C++
	//support for the current compiler, as marshalling may be occurring between two
	//assemblies, one of which has been compiled with a C++11 compliant compiler, and the
	//other which has not. This object needs to have the same virtual function table in
	//both modules in order to allow them to communicate.
	virtual void MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const = 0;

private:
	//Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		//Obtain a pointer to the first data element in the source object
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		const ElementType* sourceData = sourceObject.data();
#else
		const ElementType* sourceData = sourceObject.empty()? 0: &sourceObject[0];
#endif

		//Marshal the contents of this container directly into the target object. We move
		//the elements if the supplied source is a non-const rvalue reference, otherwise
		//we copy the elements.
		if(Internal::is_non_const_rvalue_reference<MARSHALSUPPORT_UNIVERSALREFERENCE(FullType)>::value)
		{
			//Note that we explicitly cast away the const modifier from our array pointer
			//if it is present. This is actually totally redundant if the specified type
			//is a non-const rvalue reference, but in order to make this code compile with
			//all reference types, we provide this const cast. The cast will always be
			//redundant when this code path executes, and this branch will never be called
			//and should be optimized away by the compiler when the specified type isn't a
			//non-const rvalue reference.
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), const_cast<ElementType*>(sourceData), sourceObject.size());
		}
		else
		{
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), sourceData, sourceObject.size());
		}
	}
};
template<class ElementType, class Alloc>
IMarshalTarget<std::vector<ElementType, Alloc>, false, true>::~IMarshalTarget() { }

//----------------------------------------------------------------------------------------
template<class Alloc>
class IMarshalTarget<std::vector<bool, Alloc>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalTarget() = 0;

	//Marshalling methods
	void MarshalFrom(const std::vector<bool, Alloc>& sourceObject) const
	{
		//Since the C++ standard defines a rather poor specialization of std::vector for
		//the bool type which bitpacks values, and therefore can't provide a reference to
		//its underlying data members like the vector container normally can, we need to
		//repack the elements of this boolean vector here into a flat array so that we can
		//marshal it.
		size_t sourceObjectLength = sourceObject.size();
		bool* itemArray = new bool[sourceObjectLength];
		for(size_t i = 0; i < sourceObjectLength; ++i)
		{
			itemArray[i] = sourceObject[i];
		}

		//Marshal the vector data to the target object
		MarshalFrom(itemArray, sourceObjectLength);

		//Delete the temporary array of unpacked vector data
		delete[] itemArray;
	}

protected:
	//Marshalling methods
	virtual void MarshalFrom(const bool* sourceData, size_t sourceDataLength) const = 0;
};
template<class Alloc>
IMarshalTarget<std::vector<bool, Alloc>, false, true>::~IMarshalTarget() { }

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class IMarshalTarget<std::array<ElementType, ArraySize>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalTarget() = 0;

	//Marshalling methods
	inline void MarshalFrom(const std::array<ElementType, ArraySize>& sourceObject) const
	{
		MarshalFromInternal(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline void MarshalFrom(std::array<ElementType, ArraySize>&& sourceObject) const
	{
		MarshalFromInternal(std::move(sourceObject));
	}
#endif

protected:
	//Marshalling methods
	virtual void MarshalFrom(size_t elementByteSize, const ElementType* sourceData) const = 0;
	//Note that although this virtual function is only required if C++11 support exists in
	//order to perform a move, we still need to provide it regardless of the level of C++
	//support for the current compiler, as marshalling may be occurring between two
	//assemblies, one of which has been compiled with a C++11 compliant compiler, and the
	//other which has not. This object needs to have the same virtual function table in
	//both modules in order to allow them to communicate.
	virtual void MarshalFrom(size_t elementByteSize, ElementType* sourceData) const = 0;

private:
	//Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		//Obtain a pointer to the first data element in the source object
		const ElementType* sourceData = sourceObject.data();

		//Marshal the contents of this container directly into the target object. We move
		//the elements if the supplied source is a non-const rvalue reference, otherwise
		//we copy the elements.
		if(Internal::is_non_const_rvalue_reference<MARSHALSUPPORT_UNIVERSALREFERENCE(FullType)>::value)
		{
			//Note that we explicitly cast away the const modifier from our array pointer
			//if it is present. This is actually totally redundant if the specified type
			//is a non-const rvalue reference, but in order to make this code compile with
			//all reference types, we provide this const cast. The cast will always be
			//redundant when this code path executes, and this branch will never be called
			//and should be optimized away by the compiler when the specified type isn't a
			//non-const rvalue reference.
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), const_cast<ElementType*>(sourceData));
		}
		else
		{
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), sourceData);
		}
	}
};
template<class ElementType, size_t ArraySize>
IMarshalTarget<std::array<ElementType, ArraySize>, false, true>::~IMarshalTarget() { }
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class IMarshalTarget<std::basic_string<ElementType, traits, Alloc>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalTarget() = 0;

	//Marshalling methods
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
	//Marshalling methods
	virtual void MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const = 0;
	//Note that although this virtual function is only required if C++11 support exists in
	//order to perform a move, we still need to provide it regardless of the level of C++
	//support for the current compiler, as marshalling may be occurring between two
	//assemblies, one of which has been compiled with a C++11 compliant compiler, and the
	//other which has not. This object needs to have the same virtual function table in
	//both modules in order to allow them to communicate.
	virtual void MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const = 0;

private:
	//Marshalling methods
	template<class FullType>
	void MarshalFromInternal(MARSHALSUPPORT_UNIVERSALREFERENCE(FullType) sourceObject) const
	{
		//Obtain a pointer to the first data element in the source object
		const ElementType* sourceData = sourceObject.data();

		//Marshal the contents of this container directly into the target object. We move
		//the elements if the supplied source is a non-const rvalue reference, otherwise
		//we copy the elements.
		if(Internal::is_non_const_rvalue_reference<MARSHALSUPPORT_UNIVERSALREFERENCE(FullType)>::value)
		{
			//Note that we explicitly cast away the const modifier from our array pointer
			//if it is present. This is actually totally redundant if the specified type
			//is a non-const rvalue reference, but in order to make this code compile with
			//all reference types, we provide this const cast. The cast will always be
			//redundant when this code path executes, and this branch will never be called
			//and should be optimized away by the compiler when the specified type isn't a
			//non-const rvalue reference.
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), const_cast<ElementType*>(sourceData), sourceObject.size());
		}
		else
		{
			MarshalFrom(sizeof(typename Internal::get_last_nested_container_element_type<ElementType>::type), sourceData, sourceObject.size());
		}
	}
};
template<class ElementType, class traits, class Alloc>
IMarshalTarget<std::basic_string<ElementType, traits, Alloc>, false, true>::~IMarshalTarget() { }

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace MarshalSupport
#endif
