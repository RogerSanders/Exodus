#ifndef __MARSHALTARGET_H__
#define __MARSHALTARGET_H__
#include "IMarshalTarget.h"
#include "MarshalPreprocessorMacros.h"
#include "IsLastNestedContainerElement.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#include "IsSTLContainerNestedElementMarshallableOrSameSize.h"
#include "HasMarshalConstructor.h"
#include "ClearSTLContainer.h"
#include "MarshalObjectHelper.h"
#include "RecomposeSTLContainer.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <memory.h>
#endif
#include <cassert>
namespace MarshalSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about using "this" pointer in initializer list. Our usage here is safe,
//and guaranteed by the standard. See section 12.6.2 [class.base.init] paragraph 7 of the
//standard for clarification.
#pragma warning(disable:4355)
//Disable warning about our use of type traits causing conditional expressions to be
//constant. The code behaves as intended, and the compiler is free to optimize away the
//dead branch.
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
namespace Internal {
template<class ContainerType, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class MarshalTargetInternal :public IMarshalTarget<ContainerType, false, IsThisOrNextElementLastElement>
{
public:
	//Constructors
	MarshalTargetInternal(ContainerType& targetObject)
	:_targetObject(targetObject)
	{ }

protected:
	//Marshalling methods
	virtual void MarshalFromFlatArrays(size_t elementByteSize, void* itemArray, const size_t* elementSizeArray, Internal::ISTLObjectKeyMarshallerBase* const* keyMarshallerArray) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		//Ensure the target container is cleared of any current data
		Internal::ClearSTLContainer(_targetObject);

		//Recompose our flat arrays into the target container
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		Internal::RecomposeSTLContainerHelper<Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, _targetObject);
	}

private:
	//Disable copying and moving
	MarshalTargetInternal(const MarshalTargetInternal& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTargetInternal& operator=(const MarshalTargetInternal& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTargetInternal(MarshalTargetInternal&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTargetInternal& operator=(MarshalTargetInternal&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	ContainerType& _targetObject;
};
} //Close namespace Internal

//----------------------------------------------------------------------------------------
template<class ContainerType, bool IsLastElement = Internal::is_last_nested_container_element<ContainerType>::value, bool IsThisOrNextElementLastElement = Internal::is_this_or_nested_element_last_nested_container_element<ContainerType>::value>
class MarshalTarget
{ };

//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalTarget<ContainerType, false, false> :public Internal::MarshalTargetInternal<ContainerType, false>
{
public:
	//Constructors
	MarshalTarget(ContainerType& targetObject)
	:Internal::MarshalTargetInternal<ContainerType, false>(targetObject)
	{ }

private:
	//Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif
};

//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalTarget<ContainerType, false, true> :public Internal::MarshalTargetInternal<ContainerType, true>
{
public:
	//Constructors
	MarshalTarget(ContainerType& targetObject)
	:Internal::MarshalTargetInternal<ContainerType, true>(targetObject)
	{ }

private:
	//Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif
};

//----------------------------------------------------------------------------------------
template<class ContainerType>
class MarshalTarget<ContainerType, true, true> :public IMarshalTarget<ContainerType, true, true>
{
public:
	//Constructors
	MarshalTarget(ContainerType& targetObject)
	:_targetObject(targetObject)
	{ }

protected:
	//Marshalling methods
	virtual void MarshalDirectFromSource(size_t elementByteSize, const ContainerType& sourceObject) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		//Marshal the object, performing a copy, or invoking a marshal constructor as
		//required.
		Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(sourceObject, _targetObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void MarshalDirectFromSource(size_t elementByteSize, ContainerType&& sourceObject) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (ContainerType*)0));

		//Marshal the object, performing a move, or invoking a marshal constructor as
		//required.
		Internal::MarshalObjectHelper<ContainerType>::MarshalObjectToExistingObject(std::move(sourceObject), _targetObject);
	}
#endif

private:
	//Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	ContainerType& _targetObject;
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalTarget<std::vector<ElementType, Alloc>, false, true> :public IMarshalTarget<std::vector<ElementType, Alloc>, false, true>
{
public:
	//Constructors
	MarshalTarget(std::vector<ElementType, Alloc>& targetObject)
	:_targetObject(targetObject)
	{ }

protected:
	//Marshalling methods
	virtual void MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		//Either marshal or copy the elements to the target array depending on whether a
		//custom marshaller is implemented for the target elements.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Ensure the target vector is empty, with enough space reserved for the number
			//of elements we're about to insert in it.
			_targetObject.clear();
			_targetObject.reserve(sourceDataLength);

			//Marshal each element of the supplied array to the target vector. Note that
			//since the object has a custom marshal constructor, it's possible the size
			//and layout of the object differs on each side of the boundary. We've
			//received a pointer to an array of source objects to marshal, but we can't
			//index that array using our size of the object here, since it may differ from
			//that of the sender. To account for this, we calculate the address of each
			//object in the array using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			for(size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(*nextSourceObject, _targetObject);
				nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			//Directly initialize the target vector with the supplied array of data
			_targetObject.assign(sourceData, sourceData + sourceDataLength);
		}
	}
	virtual void MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::vector<ElementType, Alloc>*)0));

		//Either marshal or move the elements to the target array depending on whether a
		//custom marshaller is implemented for the target elements.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Ensure the target vector is empty, with enough space reserved for the number
			//of elements we're about to insert in it.
			_targetObject.clear();
			_targetObject.reserve(sourceDataLength);

			//Marshal each element of the supplied array to the target vector. Note that
			//since the object has a custom marshal constructor, it's possible the size
			//and layout of the object differs on each side of the boundary. We've
			//received a pointer to an array of source objects to marshal, but we can't
			//index that array using our size of the object here, since it may differ from
			//that of the sender. To account for this, we calculate the address of each
			//object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for(size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), _targetObject);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			//Directly initialize the target vector with the supplied array of data. Note
			//that we only perform the assignment using move iterators where the target
			//elements use a non-trivial type. This is done as an optimization. Although
			//the assign method in theory has all the necessary information to optimize
			//copying an array of standard layout types in one process using memcpy, in
			//VS2013 it was observed that the move iterator wrappers prevent this
			//optimization occurring. We avoid using move iterators in this case to
			//achieve maximum efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if(!std::is_trivial<ElementType>::value)
			{
				_targetObject.assign(std::make_move_iterator(sourceData), std::make_move_iterator(sourceData + sourceDataLength));
			}
			else
#endif
			{
				_targetObject.assign(sourceData, sourceData + sourceDataLength);
			}
		}
	}

private:
	//Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	std::vector<ElementType, Alloc>& _targetObject;
};

//----------------------------------------------------------------------------------------
template<class Alloc>
class MarshalTarget<std::vector<bool, Alloc>, false, true> :public IMarshalTarget<std::vector<bool, Alloc>, false, true>
{
public:
	//Constructors
	MarshalTarget(std::vector<bool, Alloc>& targetObject)
	:_targetObject(targetObject)
	{ }

protected:
	//Marshalling methods
	virtual void MarshalFrom(const bool* sourceData, size_t sourceDataLength) const
	{
		_targetObject.assign(sourceData, sourceData + sourceDataLength);
	}

private:
	//Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	std::vector<bool, Alloc>& _targetObject;
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class MarshalTarget<std::array<ElementType, ArraySize>, false, true> :public IMarshalTarget<std::array<ElementType, ArraySize>, false, true>
{
public:
	//Constructors
	MarshalTarget(std::array<ElementType, ArraySize>& targetObject)
	:_targetObject(targetObject)
	{ }

protected:
	//Marshalling methods
	virtual void MarshalFrom(size_t elementByteSize, const ElementType* sourceData) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		//Either marshal or copy the elements to the target array depending on whether a
		//custom marshaller is implemented for the target elements.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Marshal each element of the supplied array to the target array. Note that
			//since the object has a custom marshal constructor, it's possible the size
			//and layout of the object differs on each side of the boundary. We've
			//received a pointer to an array of source objects to marshal, but we can't
			//index that array using our size of the object here, since it may differ from
			//that of the sender. To account for this, we calculate the address of each
			//object in the array using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			for(size_t i = 0; i < ArraySize; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(*nextSourceObject, _targetObject[i]);
				nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else if(!std::is_trivial<ElementType>::value)
		{
			for(size_t i = 0; i < ArraySize; ++i)
			{
				_targetObject[i] = sourceData[i];
			}
		}
		else
		{
			memcpy((void*)_targetObject.data(), (const void*)sourceData, ArraySize * sizeof(ElementType));
		}
	}
	virtual void MarshalFrom(size_t elementByteSize, ElementType* sourceData) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::array<ElementType, ArraySize>*)0));

		//Either marshal or move the elements to the target array depending on whether a
		//custom marshaller is implemented for the target elements.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Marshal each element of the supplied array to the target array. Note that
			//since the object has a custom marshal constructor, it's possible the size
			//and layout of the object differs on each side of the boundary. We've
			//received a pointer to an array of source objects to marshal, but we can't
			//index that array using our size of the object here, since it may differ from
			//that of the sender. To account for this, we calculate the address of each
			//object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for(size_t i = 0; i < ArraySize; ++i)
			{
				Internal::MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(std::move(*nextSourceObject), _targetObject[i]);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else if(!std::is_trivial<ElementType>::value)
		{
			for(size_t i = 0; i < ArraySize; ++i)
			{
				_targetObject[i] = std::move(sourceData[i]);
			}
		}
		else
		{
			memcpy((void*)_targetObject.data(), (const void*)sourceData, ArraySize * sizeof(ElementType));
		}
	}

private:
	//Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;

private:
	std::array<ElementType, ArraySize>& _targetObject;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class MarshalTarget<std::basic_string<ElementType, traits, Alloc>, false, true> :public IMarshalTarget<std::basic_string<ElementType, traits, Alloc>, false, true>
{
public:
	//Constructors
	MarshalTarget(std::basic_string<ElementType, traits, Alloc>& targetObject)
	:_targetObject(targetObject)
	{ }

protected:
	//Marshalling methods
	virtual void MarshalFrom(size_t elementByteSize, const ElementType* sourceData, size_t sourceDataLength) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::basic_string<ElementType, traits, Alloc>*)0));

		//Either marshal or copy the elements to the target array depending on whether a
		//custom marshaller is implemented for the target elements.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Ensure the target vector is empty, with enough space reserved for the number
			//of elements we're about to insert in it.
			_targetObject.clear();
			_targetObject.reserve(sourceDataLength);

			//Marshal each element of the supplied array to the target vector. Note that
			//since the object has a custom marshal constructor, it's possible the size
			//and layout of the object differs on each side of the boundary. We've
			//received a pointer to an array of source objects to marshal, but we can't
			//index that array using our size of the object here, since it may differ from
			//that of the sender. To account for this, we calculate the address of each
			//object in the array using the object byte size specified by the sender.
			const ElementType* nextSourceObject = sourceData;
			for(size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(*nextSourceObject, _targetObject);
				nextSourceObject = reinterpret_cast<const ElementType*>(reinterpret_cast<const unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			//Directly initialize the target vector with the supplied array of data
			_targetObject.assign(sourceData, sourceData + sourceDataLength);
		}
	}
	virtual void MarshalFrom(size_t elementByteSize, ElementType* sourceData, size_t sourceDataLength) const
	{
		//Ensure the size of the innermost element is the same as expected, or that a
		//custom marshaller has been supplied. The object being the correct size isn't a
		//guarantee that marshalling is being used correctly, but if the object size is
		//different, it's definitely being used incorrectly.
		assert(Internal::IsSTLContainerNestedElementMarshallableOrSameSize(elementByteSize, (std::basic_string<ElementType, traits, Alloc>*)0));

		//Either marshal or move the elements to the target array depending on whether a
		//custom marshaller is implemented for the target elements.
		if(Internal::has_marshal_constructor<ElementType>::value)
		{
			//Ensure the target vector is empty, with enough space reserved for the number
			//of elements we're about to insert in it.
			_targetObject.clear();
			_targetObject.reserve(sourceDataLength);

			//Marshal each element of the supplied array to the target vector. Note that
			//since the object has a custom marshal constructor, it's possible the size
			//and layout of the object differs on each side of the boundary. We've
			//received a pointer to an array of source objects to marshal, but we can't
			//index that array using our size of the object here, since it may differ from
			//that of the sender. To account for this, we calculate the address of each
			//object in the array using the object byte size specified by the sender.
			ElementType* nextSourceObject = sourceData;
			for(size_t i = 0; i < sourceDataLength; ++i)
			{
				Internal::MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), _targetObject);
				nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
			}
		}
		else
		{
			//Directly initialize the target vector with the supplied array of data. Note
			//that we only perform the assignment using move iterators where the target
			//elements use a non-trivial type. This is done as an optimization. Although
			//the assign method in theory has all the necessary information to optimize
			//copying an array of standard layout types in one process using memcpy, in
			//VS2013 it was observed that the move iterator wrappers prevent this
			//optimization occurring. We avoid using move iterators in this case to
			//achieve maximum efficiency.
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
			if(!std::is_trivial<ElementType>::value)
			{
				_targetObject.assign(std::make_move_iterator(sourceData), std::make_move_iterator(sourceData + sourceDataLength));
			}
			else
#endif
			{
				_targetObject.assign(sourceData, sourceData + sourceDataLength);
			}
		}
	}

private:
	//Disable copying and moving
	MarshalTarget(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(const MarshalTarget& source) MARSHALSUPPORT_DELETEMETHOD;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	MarshalTarget(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
	MarshalTarget& operator=(MarshalTarget&& source) MARSHALSUPPORT_DELETEMETHOD;
#endif

private:
	std::basic_string<ElementType, traits, Alloc>& _targetObject;
};

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace MarshalSupport
#endif
