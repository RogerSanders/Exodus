#ifndef __IMARSHALSOURCE_H__
#define __IMARSHALSOURCE_H__
#include "MarshalPreprocessorMacros.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#include "IsSTLContainerNestedElementMarshallableOrSameSize.h"
#include "HasMarshalConstructor.h"
#include "ClearSTLContainer.h"
#include "MarshalObjectHelper.h"
#include "RecomposeSTLContainer.h"
#include "ISTLObjectKeyMarshallerBase.h"
#include <vector>
#include <string>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#include <type_traits>
#endif
#include <cassert>
namespace MarshalSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about conditional expressions being constant. The code behaves as
//intended, and the compiler is free to optimize away any dead code branches.
#pragma warning(disable:4127)
#endif

//##TODO## Add support for the std::tuple and std::pair types. We can do this by routing
//leading elements through key marshallers, and just storing the last element in our
//decomposed item array.
//##TODO## Add support for std::bitset?
//----------------------------------------------------------------------------------------
namespace Internal {
template<class ContainerType>
class IMarshalSourceInternal
{
public:
	//Constructors
	inline virtual ~IMarshalSourceInternal() = 0;

	//Marshalling methods
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
	//Marshalling methods
	virtual void RetrieveData(size_t& elementByteSize, void*& itemArray, const size_t*& elementSizeArray, Internal::ISTLObjectKeyMarshallerBase* const*& keyMarshallerArray, bool allowMove) const = 0;
	virtual void FreeDataArrays(void* itemArray, const size_t* elementSizeArray, Internal::ISTLObjectKeyMarshallerBase* const* keyMarshallerArray) const = 0;

private:
	//Helper classes
	class ScopedFreeDataArrayHelper
	{
	public:
		//Constructors
		ScopedFreeDataArrayHelper(const IMarshalSourceInternal& atargetMarshaller)
		:targetMarshaller(atargetMarshaller), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
		{ }
		~ScopedFreeDataArrayHelper()
		{
			//Release the allocated data arrays
			targetMarshaller.FreeDataArrays(itemArray, elementSizeArray, keyMarshallerArray);
		}

	public:
		const IMarshalSourceInternal& targetMarshaller;
		void* itemArray;
		const size_t* elementSizeArray;
		Internal::ISTLObjectKeyMarshallerBase* const* keyMarshallerArray;
	};
#ifndef MARSHALSUPPORT_CPP11SUPPORTED
	//This friend declaration is not required in C++11, but is added for earlier versions
	//of the C++ standard due to ambiguity and contradiction in prior versions regarding
	//nested class access to its enclosing class.
	friend class ScopedFreeDataArrayHelper;
#endif

private:
	//Marshalling methods
	template<bool AllowMove>
	ContainerType MarshalToInternal() const
	{
		//Retrieve arrays to load the decomposed object data from. Note that we give
		//ownership of the arrays to a helper object which frees the data in its
		//destructor, to ensure we don't leak memory if an exception is triggered during
		//the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.keyMarshallerArray, AllowMove);

		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		//Recompose our flat arrays into a new object
		ContainerType targetObject;
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainerHelper<Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);

		//Return the marshalled object to the caller
		return targetObject;
	}
	template<bool AllowMove>
	void MarshalToInternal(ContainerType& targetObject) const
	{
		//Retrieve arrays to load the decomposed object data from. Note that we give
		//ownership of the arrays to a helper object which frees the data in its
		//destructor, to ensure we don't leak memory if an exception is triggered during
		//the marshalling process.
		ScopedFreeDataArrayHelper arrayHelper(*this);
		size_t elementByteSize;
		RetrieveData(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.keyMarshallerArray, AllowMove);

		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		//Ensure the target container is cleared of any current data
		Internal::ClearSTLContainer(targetObject);

		//Recompose our flat arrays into the target object
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainerHelper<Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>::RecomposeSTLContainer(elementByteSize, arrayHelper.itemArray, arrayHelper.elementSizeArray, arrayHelper.keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}
};
template<class ContainerType>
IMarshalSourceInternal<ContainerType>::~IMarshalSourceInternal() { }
} //Close namespace Internal

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class IMarshalSource
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalSource<ContainerType, false, false> :public Internal::IMarshalSourceInternal<ContainerType>
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalSource<ContainerType, false, true> :public Internal::IMarshalSourceInternal<ContainerType>
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class IMarshalSource<ContainerType, true, true>
{
public:
	//Constructors
	inline virtual ~IMarshalSource() = 0;

	//Marshalling methods
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
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ContainerType*& sourceData) const = 0;

private:
	//Marshalling methods
	template<bool AllowMove>
	ContainerType MarshalToInternal() const
	{
		//Retrieve a reference to the object being marshalled
		size_t elementByteSize;
		const ContainerType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		//Marshal the object, performing a move, copy, or invoking a marshal constructor
		//as required.
		if(sourceDataMovable && AllowMove)
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
		//Retrieve a reference to the object being marshalled
		size_t elementByteSize;
		const ContainerType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		//Marshal the object, performing a move, copy, or invoking a marshal constructor
		//as required.
		if(sourceDataMovable && AllowMove)
		{
			Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(const_cast<ContainerType&>(*sourceData)), targetObject);
		}
		else
		{
			Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(*sourceData, targetObject);
		}
	}
};
template<class ContainerType>
IMarshalSource<ContainerType, true, true>::~IMarshalSource() { }

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class IMarshalSource<std::vector<ElementType, Alloc>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalSource() = 0;

	//Marshalling methods
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

protected:
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ElementType*& sourceData, size_t& sourceDataLength) const = 0;

private:
	//Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::vector<ElementType, Alloc>& targetObject) const
	{
		//Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		size_t sourceDataLength;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData, sourceDataLength);

		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't
		//a guarantee that marshalling is being used correctly, but if the object size
		//is different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		//If the target element type has a custom marshaller, marshal the elements one
		//by one, otherwise copy the contents of the source container directly into
		//the target object.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Ensure the target vector is empty, with enough space reserved for the
			//number of elements we're about to insert in it.
			targetObject.clear();
			targetObject.reserve(sourceDataLength);

			//Marshal the object. Note that since the object has a custom marshal
			//constructor, it's possible the size and layout of the object differs on
			//each side of the boundary. We've received a pointer to an array of
			//source objects to marshal, but we can't index that array using our size
			//of the object here, since it may differ from that of the sender. To
			//account for this, we calculate the address of each object in the array
			//using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			if(sourceDataMovable && AllowMove)
			{
				for(size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(const_cast<ElementType&>(*nextSourceObject)), targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
			else
			{
				for(size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(*nextSourceObject, targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
		}
		else
		{
			//Directly initialize the target vector with the supplied array of data,
			//performing a move operation if one is available and permitted. Note that
			//in addition, we only use move iterators where the target elements use a
			//non-trivial type. This is done as an optimization. Although the assign
			//method in theory has all the necessary information to optimize copying
			//an array of standard layout types in one process using memcpy, in VS2013
			//it was observed that the move iterator wrappers prevent this
			//optimization occurring. We avoid using move iterators in this case to
			//achieve maximum efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if(!std::is_trivial<ElementType>::value && sourceDataMovable && AllowMove)
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
template<class ElementType, class Alloc>
IMarshalSource<std::vector<ElementType, Alloc>, false, true>::~IMarshalSource() { }

//----------------------------------------------------------------------------------------
template<class Alloc>
class IMarshalSource<std::vector<bool, Alloc>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalSource() = 0;

	//Marshalling methods
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
	//Marshalling methods
	virtual void RetrieveData(const bool*& sourceData, size_t& sourceDataLength) const = 0;
	virtual void FreeDataArrays(const bool* sourceData) const = 0;

private:
	//Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::vector<bool, Alloc>& targetObject) const
	{
		//Retrieve a pointer to the raw array contents in the target object
		const bool* sourceData;
		size_t sourceDataLength;
		RetrieveData(sourceData, sourceDataLength);

		//Copy the contents of the source container directly into the target object
		targetObject.assign(sourceData, sourceData + sourceDataLength);

		//Release the allocated data arrays
		FreeDataArrays(sourceData);
	}
};
template<class Alloc>
IMarshalSource<std::vector<bool, Alloc>, false, true>::~IMarshalSource() {}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class IMarshalSource<std::array<ElementType, ArraySize>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalSource() = 0;

	//Marshalling methods
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

protected:
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ElementType*& sourceData) const = 0;

private:
	//Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::array<ElementType, ArraySize>& targetObject) const
	{
		//Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData);

		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		//If the target element type has a custom marshaller, marshal the elements one by
		//one, otherwise copy the contents of the source container directly into the
		//target object.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Marshal the object. Note that since the object has a custom marshal
			//constructor, it's possible the size and layout of the object differs on each
			//side of the boundary. We've received a pointer to an array of source objects
			//to marshal, but we can't index that array using our size of the object here,
			//since it may differ from that of the sender. To account for this, we
			//calculate the address of each object in the array using the object byte size
			//specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			if(sourceDataMovable && AllowMove)
			{
				for(size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(const_cast<ElementType&>(*nextSourceObject)), targetObject[i]);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
			else
			{
				for(size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*nextSourceObject, targetObject[i]);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
		}
		else
		{
			//Directly initialize the target array with the supplied array of data,
			//performing a direct memory copy if safe, or performing a move operation if
			//one is available and permitted.
			if(std::is_trivial<ElementType>::value)
			{
				memcpy((void*)targetObject.data(), (const void*)sourceData, ArraySize * sizeof(ElementType));
			}
			else if(sourceDataMovable && AllowMove)
			{
				for(size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(const_cast<ElementType&>(sourceData[i])), targetObject[i]);
				}
			}
			else
			{
				for(size_t i = 0; i < ArraySize; ++i)
				{
					Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(sourceData[i], targetObject[i]);
				}
			}
		}
	}
};
template<class ElementType, size_t ArraySize>
IMarshalSource<std::array<ElementType, ArraySize>, false, true>::~IMarshalSource() { }
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class IMarshalSource<std::basic_string<ElementType, traits, Alloc>, false, true>
{
public:
	//Constructors
	inline virtual ~IMarshalSource() = 0;

	//Marshalling methods
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
	//Marshalling methods
	virtual bool RetrieveData(size_t& elementByteSize, const ElementType*& sourceData, size_t& sourceDataLength) const = 0;

private:
	//Marshalling methods
	template<bool AllowMove>
	void MarshalToInternal(std::basic_string<ElementType, traits, Alloc>& targetObject) const
	{
		//Retrieve a pointer to the raw array contents in the target object
		size_t elementByteSize;
		const ElementType* sourceData;
		size_t sourceDataLength;
		bool sourceDataMovable = RetrieveData(elementByteSize, sourceData, sourceDataLength);

		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::basic_string<ElementType, traits, Alloc>*)0));

		//If the target element type has a custom marshaller, marshal the elements one by
		//one, otherwise copy the contents of the source container directly into the
		//target object.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Ensure the target vector is empty, with enough space reserved for the number
			//of elements we're about to insert in it.
			targetObject.clear();
			targetObject.reserve(sourceDataLength);

			//Marshal the object. Note that since the object has a custom marshal
			//constructor, it's possible the size and layout of the object differs on each
			//side of the boundary. We've received a pointer to an array of source objects
			//to marshal, but we can't index that array using our size of the object here,
			//since it may differ from that of the sender. To account for this, we
			//calculate the address of each object in the array using the object byte size
			//specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			if(sourceDataMovable && AllowMove)
			{
				for(size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(const_cast<ElementType&>(*nextSourceObject)), targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
			else
			{
				for(size_t i = 0; i < sourceDataLength; ++i)
				{
					Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(*nextSourceObject, targetObject);
					nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
				}
			}
		}
		else
		{
			//Directly initialize the target vector with the supplied array of data,
			//performing a move operation if one is available and permitted. Note that in
			//addition, we only use move iterators where the target elements use a
			//non-trivial type. This is done as an optimization. Although the assign
			//method in theory has all the necessary information to optimize copying an
			//array of standard layout types in one process using memcpy, in VS2013 it was
			//observed that the move iterator wrappers prevent this optimization
			//occurring. We avoid using move iterators in this case to achieve maximum
			//efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if(!std::is_trivial<ElementType>::value && sourceDataMovable && AllowMove)
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
template<class ElementType, class traits, class Alloc>
IMarshalSource<std::basic_string<ElementType, traits, Alloc>, false, true>::~IMarshalSource() { }

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace MarshalSupport
#endif
