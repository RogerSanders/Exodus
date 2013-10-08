#ifndef __STLOBJECTTARGET_H__
#define __STLOBJECTTARGET_H__
#include "InteropPreprocessorMacros.h"
#include "ISTLObjectTarget.h"
#include "DecomposedSTLContainerHasKeys.h"
#include "CreateSTLContainerKeyMarshallers.h"
#include "DeleteSTLContainerKeyMarshallers.h"
#include "CreateSTLContainerItemArray.h"
#include "ClearSTLContainer.h"
#include "RecomposeSTLContainer.h"
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <memory.h>
#endif
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ContainerType>
class STLObjectTarget :public ISTLObjectTarget<ContainerType>
{
public:
	//Constructors
	STLObjectTarget(ContainerType& atargetObject)
		:targetObject(atargetObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectTarget()
	{
		delete[] itemArray;
		delete[] elementSizeArray;
		if(HasKeyMarshallers())
		{
			DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (ContainerType*)0);
			delete[] keyMarshallerArray;
		}
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Allocate our flat arrays to hold the decomposed object data
		delete[] itemArray;
		delete[] elementSizeArray;
		itemArray = CreateSTLContainerItemArray(itemArraySize, &targetObject);
		elementSizeArray = new size_t[elementSizeArraySize];

		//Allocate key marshallers for any keyed containers in the source object
		if(HasKeyMarshallers())
		{
			INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (ContainerType*)0);
			if(keyMarshallerArray != 0)
			{
				DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (ContainerType*)0);
			}
			else
			{
				keyMarshallerArray = new ISTLObjectKeyMarshallerBase*[nestingDepth];
			}
			CreateSTLContainerKeyMarshallers(keyMarshallerArray, 0, (ContainerType*)0);
		}

		//Return the arrays to receive the decomposed item data to the caller
		aitemArray = itemArray;
		aelementSizeArray = elementSizeArray;
		akeyMarshallerArray = keyMarshallerArray;
	}
	virtual void MarshalFromFlatArrays() const
	{
		//Ensure the target container is cleared of any current data
		ClearSTLContainer(targetObject);

		//Recompose our flat arrays into the target container
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}

private:
	//Marshalling methods
	INTEROPSUPPORT_CONSTEXPR bool HasKeyMarshallers() const
	{
		return DecomposedSTLContainerHasKeys((ContainerType*)0);
	}

private:
	mutable void* itemArray;
	mutable size_t* elementSizeArray;
	mutable ISTLObjectKeyMarshallerBase** keyMarshallerArray;
	mutable ContainerType& targetObject;
};

//----------------------------------------------------------------------------------------
template<class ElementType>
class STLObjectTarget<std::vector<ElementType>> :public ISTLObjectTarget<std::vector<ElementType>>
{
public:
	//Constructors
	STLObjectTarget(std::vector<ElementType>& atargetObject)
		:targetObject(atargetObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectTarget()
	{
		delete[] itemArray;
		delete[] elementSizeArray;
		if(HasKeyMarshallers())
		{
			DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::vector<ElementType>*)0);
			delete[] keyMarshallerArray;
		}
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Allocate our flat arrays to hold the decomposed object data
		delete[] itemArray;
		delete[] elementSizeArray;
		itemArray = CreateSTLContainerItemArray(itemArraySize, &targetObject);
		elementSizeArray = new size_t[elementSizeArraySize];

		//Allocate key marshallers for any keyed containers in the source object
		if(HasKeyMarshallers())
		{
			INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::vector<ElementType>*)0);
			if(keyMarshallerArray != 0)
			{
				DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::vector<ElementType>*)0);
			}
			else
			{
				keyMarshallerArray = new ISTLObjectKeyMarshallerBase*[nestingDepth];
			}
			CreateSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::vector<ElementType>*)0);
		}

		//Return the arrays to receive the decomposed item data to the caller
		aitemArray = itemArray;
		aelementSizeArray = elementSizeArray;
		akeyMarshallerArray = keyMarshallerArray;
	}
	virtual void MarshalFrom(const ElementType* sourceData, size_t sourceDataLength) const
	{
		targetObject.assign(sourceData, sourceData + sourceDataLength);
	}
	virtual void MarshalFromFlatArrays() const
	{
		//Recompose our flat arrays into the target container
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}

private:
	//Marshalling methods
	INTEROPSUPPORT_CONSTEXPR bool HasKeyMarshallers() const
	{
		return DecomposedSTLContainerHasKeys((std::vector<ElementType>*)0);
	}

private:
	mutable void* itemArray;
	mutable size_t* elementSizeArray;
	mutable ISTLObjectKeyMarshallerBase** keyMarshallerArray;
	mutable std::vector<ElementType>& targetObject;
};

//----------------------------------------------------------------------------------------
template<>
class STLObjectTarget<std::vector<bool>> :public ISTLObjectTarget<std::vector<bool>>
{
public:
	//Constructors
	STLObjectTarget(std::vector<bool>& atargetObject)
		:targetObject(atargetObject)
	{ }

protected:
	//Marshalling methods
	virtual void MarshalFrom(const bool* sourceData, size_t sourceDataLength) const
	{
		targetObject.assign(sourceData, sourceData + sourceDataLength);
	}

private:
	mutable std::vector<bool>& targetObject;
};

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class STLObjectTarget<std::array<ElementType, ArraySize>> :public ISTLObjectTarget<std::vector<ElementType>>
{
public:
	//Constructors
	STLObjectTarget(std::array<ElementType, ArraySize>& atargetObject)
		:targetObject(atargetObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectTarget()
	{
		delete[] itemArray;
		delete[] elementSizeArray;
		if(HasKeyMarshallers())
		{
			DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::array<ElementType, ArraySize>*)0);
			delete[] keyMarshallerArray;
		}
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Allocate our flat arrays to hold the decomposed object data
		delete[] itemArray;
		delete[] elementSizeArray;
		itemArray = CreateSTLContainerItemArray(itemArraySize, &targetObject);
		elementSizeArray = new size_t[elementSizeArraySize];

		//Allocate key marshallers for any keyed containers in the source object
		if(HasKeyMarshallers())
		{
			INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::array<ElementType, ArraySize>*)0);
			if(keyMarshallerArray != 0)
			{
				DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::array<ElementType, ArraySize>*)0);
			}
			else
			{
				keyMarshallerArray = new ISTLObjectKeyMarshallerBase*[nestingDepth];
			}
			CreateSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::array<ElementType, ArraySize>*)0);
		}

		//Return the arrays to receive the decomposed item data to the caller
		aitemArray = itemArray;
		aelementSizeArray = elementSizeArray;
		akeyMarshallerArray = keyMarshallerArray;
	}
	virtual void MarshalFrom(const ElementType* sourceData) const
	{
		if(std::is_pod<ElementType>::value)
		{
			memcpy((void*)targetObject.data(), (const void*)sourceData, ArraySize * sizeof(ElementType));
		}
		else
		{
			for(size_t i = 0; i < ArraySize; ++i)
			{
				targetObject[i] = sourceData[i];
			}
		}
	}
	virtual void MarshalFromFlatArrays() const
	{
		//Recompose our flat arrays into the target container
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}

private:
	//Marshalling methods
	INTEROPSUPPORT_CONSTEXPR bool HasKeyMarshallers() const
	{
		return DecomposedSTLContainerHasKeys((std::array<ElementType, ArraySize>*)0);
	}

private:
	mutable void* itemArray;
	mutable size_t* elementSizeArray;
	mutable ISTLObjectKeyMarshallerBase** keyMarshallerArray;
	mutable std::array<ElementType, ArraySize>& targetObject;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
class STLObjectTarget<std::basic_string<ElementType>> :public ISTLObjectTarget<std::wstring>
{
public:
	//Constructors
	STLObjectTarget(std::basic_string<ElementType>& atargetObject)
		:targetObject(atargetObject)
	{}

protected:
	//Marshalling methods
	virtual void MarshalFrom(const ElementType* sourceData, size_t sourceDataLength) const
	{
		//Directly load the source data into the target object
		targetObject.assign(sourceData, sourceDataLength);
	}

private:
	mutable std::basic_string<ElementType>& targetObject;
};

} //Close namespace InteropSupport
#endif
