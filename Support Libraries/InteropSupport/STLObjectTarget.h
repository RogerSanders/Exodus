#ifndef __STLOBJECTTARGET_H__
#define __STLOBJECTTARGET_H__
#include "InteropPreprocessorMacros.h"
#include "ISTLObjectTarget.h"
#include "DecomposedSTLContainerHasKeys.h"
#include "CreateSTLContainerKeyMarshallers.h"
#include "DeleteSTLContainerKeyMarshallers.h"
#include "CreateSTLContainerItemArray.h"
#include "DeleteSTLContainerItemArray.h"
#include "ClearSTLContainer.h"
#include "RecomposeSTLContainer.h"
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <memory.h>
#endif
namespace InteropSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about using "this" pointer in initializer list. Our usage here is safe, and
//guaranteed by the standard. See section 12.6.2 [class.base.init] paragraph 7 of the standard for
//clarification.
#pragma warning(disable:4355)
//Disable warning about our use of std::is_pod causing a conditional expression to be constant. The
//code behaves as intended, and the compiler is free to optimize away the dead branch.
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
template<class ContainerType>
class STLObjectTarget :public ISTLObjectTarget<ContainerType>
{
public:
	//Constructors
	STLObjectTarget(ContainerType& atargetObject)
	:selfReference(*this), targetObject(atargetObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectTarget()
	{
		selfReference.DestroyDataArrays();
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Allocate our flat arrays to hold the decomposed object data
		DeleteSTLContainerItemArray(itemArray, &targetObject);
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
	//Disable copying and moving
	STLObjectTarget(const STLObjectTarget& source);
	STLObjectTarget& operator=(const STLObjectTarget& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	STLObjectTarget(STLObjectTarget&& source);
	STLObjectTarget& operator=(STLObjectTarget&& source);
#endif

	//Cleanup methods
	virtual void DestroyDataArrays()
	{
		DeleteSTLContainerItemArray(itemArray, &targetObject);
		delete[] elementSizeArray;
		if(HasKeyMarshallers())
		{
			DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (ContainerType*)0);
			delete[] keyMarshallerArray;
		}
	}

	//Marshalling methods
	INTEROPSUPPORT_CONSTEXPR bool HasKeyMarshallers() const
	{
		return DecomposedSTLContainerHasKeys((ContainerType*)0);
	}

private:
	STLObjectTarget<ContainerType>& selfReference;
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
	:selfReference(*this), targetObject(atargetObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectTarget()
	{
		selfReference.DestroyDataArrays();
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Allocate our flat arrays to hold the decomposed object data
		DeleteSTLContainerItemArray(itemArray, &targetObject);
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
	//Disable copying and moving
	STLObjectTarget(const STLObjectTarget& source);
	STLObjectTarget& operator=(const STLObjectTarget& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	STLObjectTarget(STLObjectTarget&& source);
	STLObjectTarget& operator=(STLObjectTarget&& source);
#endif

	//Cleanup methods
	virtual void DestroyDataArrays()
	{
		DeleteSTLContainerItemArray(itemArray, &targetObject);
		delete[] elementSizeArray;
		if(HasKeyMarshallers())
		{
			DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::vector<ElementType>*)0);
			delete[] keyMarshallerArray;
		}
	}

	//Marshalling methods
	INTEROPSUPPORT_CONSTEXPR bool HasKeyMarshallers() const
	{
		return DecomposedSTLContainerHasKeys((std::vector<ElementType>*)0);
	}

private:
	STLObjectTarget<std::vector<ElementType>>& selfReference;
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
	//Disable copying and moving
	STLObjectTarget(const STLObjectTarget& source);
	STLObjectTarget& operator=(const STLObjectTarget& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	STLObjectTarget(STLObjectTarget&& source);
	STLObjectTarget& operator=(STLObjectTarget&& source);
#endif

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
	:selfReference(*this), targetObject(atargetObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectTarget()
	{
		selfReference.DestroyDataArrays();
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Allocate our flat arrays to hold the decomposed object data
		DeleteSTLContainerItemArray(itemArray, &targetObject);
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
	//Disable copying and moving
	STLObjectTarget(const STLObjectTarget& source);
	STLObjectTarget& operator=(const STLObjectTarget& source);
	STLObjectTarget(STLObjectTarget&& source);
	STLObjectTarget& operator=(STLObjectTarget&& source);

	//Cleanup methods
	virtual void DestroyDataArrays()
	{
		DeleteSTLContainerItemArray(itemArray, &targetObject);
		delete[] elementSizeArray;
		if(HasKeyMarshallers())
		{
			DeleteSTLContainerKeyMarshallers(keyMarshallerArray, 0, (std::array<ElementType, ArraySize>*)0);
			delete[] keyMarshallerArray;
		}
	}

	//Marshalling methods
	INTEROPSUPPORT_CONSTEXPR bool HasKeyMarshallers() const
	{
		return DecomposedSTLContainerHasKeys((std::array<ElementType, ArraySize>*)0);
	}

private:
	STLObjectTarget<std::array<ElementType, ArraySize>>& selfReference;
	mutable void* itemArray;
	mutable size_t* elementSizeArray;
	mutable ISTLObjectKeyMarshallerBase** keyMarshallerArray;
	mutable std::array<ElementType, ArraySize>& targetObject;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
class STLObjectTarget<std::basic_string<ElementType>> :public ISTLObjectTarget<std::basic_string<ElementType>>
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
	//Disable copying and moving
	STLObjectTarget(const STLObjectTarget& source);
	STLObjectTarget& operator=(const STLObjectTarget& source);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	STLObjectTarget(STLObjectTarget&& source);
	STLObjectTarget& operator=(STLObjectTarget&& source);
#endif

private:
	mutable std::basic_string<ElementType>& targetObject;
};

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace InteropSupport
#endif
