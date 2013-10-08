#ifndef __STLOBJECTSOURCE_H__
#define __STLOBJECTSOURCE_H__
#include "InteropPreprocessorMacros.h"
#include "ISTLObjectSource.h"
#include "DecomposedSTLContainerHasKeys.h"
#include "CreateSTLContainerKeyMarshallers.h"
#include "DeleteSTLContainerKeyMarshallers.h"
#include "CreateSTLContainerItemArray.h"
#include "DecomposeSTLContainer.h"
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ContainerType>
class STLObjectSource :public ISTLObjectSource<ContainerType>
{
public:
	//Constructors
	STLObjectSource(const ContainerType& asourceObject)
		:sourceObject(asourceObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectSource()
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
	virtual void RetrieveData(const void*& aitemArray, const size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (ContainerType*)0);

		//Calculate the required size of our flat arrays in order to hold a decomposed
		//version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		CalculateDecomposedSTLContainerSize(nestingDepth, 1, itemArraySize, elementSizeArraySize, sourceObject);

		//Allocate our flat arrays to hold the decomposed object data
		delete[] itemArray;
		delete[] elementSizeArray;
		itemArray = CreateSTLContainerItemArray(itemArraySize, &sourceObject);
		elementSizeArray = new size_t[elementSizeArraySize];

		//Allocate key marshallers for any keyed containers in the source object
		if(HasKeyMarshallers())
		{
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

		//Decompose the container hierarchy into a flat array of the innermost elements,
		//with a corresponding array of sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t keyMarshallerArrayIndex = 0;
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, sourceObject);

		//Return the arrays of decomposed item data to the caller
		aitemArray = itemArray;
		aelementSizeArray = elementSizeArray;
		akeyMarshallerArray = keyMarshallerArray;
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
	const ContainerType& sourceObject;
};

//----------------------------------------------------------------------------------------
template<class ElementType>
class STLObjectSource<std::vector<ElementType>> :public ISTLObjectSource<std::vector<ElementType>>
{
public:
	//Constructors
	STLObjectSource(const std::vector<ElementType>& asourceObject)
		:sourceObject(asourceObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectSource()
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
	virtual void RetrieveData(const ElementType*& sourceData, size_t& sourceDataLength) const
	{
		sourceData = sourceObject.empty()? 0: &sourceObject[0];
		sourceDataLength = sourceObject.size();
	}
	virtual void RetrieveData(const void*& aitemArray, const size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::vector<ElementType>*)0);

		//Calculate the required size of our flat arrays in order to hold a decomposed
		//version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		CalculateDecomposedSTLContainerSize(nestingDepth, 1, itemArraySize, elementSizeArraySize, sourceObject);

		//Allocate our flat arrays to hold the decomposed object data
		delete[] itemArray;
		delete[] elementSizeArray;
		itemArray = CreateSTLContainerItemArray(itemArraySize, &sourceObject);
		elementSizeArray = new size_t[elementSizeArraySize];

		//Allocate key marshallers for any keyed containers in the source object
		if(HasKeyMarshallers())
		{
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

		//Decompose the container hierarchy into a flat array of the innermost elements,
		//with a corresponding array of sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t keyMarshallerArrayIndex = 0;
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, sourceObject);

		//Return the arrays of decomposed item data to the caller
		aitemArray = itemArray;
		aelementSizeArray = elementSizeArray;
		akeyMarshallerArray = keyMarshallerArray;
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
	const std::vector<ElementType>& sourceObject;
};

//----------------------------------------------------------------------------------------
template<>
class STLObjectSource<std::vector<bool>> :public ISTLObjectSource<std::vector<bool>>
{
public:
	//Constructors
	STLObjectSource(const std::vector<bool>& asourceObject)
		:sourceObject(asourceObject), itemArray(0)
	{ }
	virtual ~STLObjectSource()
	{
		delete[] itemArray;
	}

protected:
	//Marshalling methods
	virtual void RetrieveData(const bool*& sourceData, size_t& sourceDataLength) const
	{
		//Since the C++ standard defines a rather poor specialization of std::vector for
		//the bool type which bitpacks values, and therefore can't provide a reference to
		//its underlying data members like the vector container normally can, we need to
		//repack the elements of this boolean vector here into a flat array so that we can
		//marshal it.
		size_t sourceObjectLength = sourceObject.size();
		delete[] itemArray;
		itemArray = new bool[sourceObjectLength];
		for(size_t i = 0; i < sourceObjectLength; ++i)
		{
			itemArray[i] = sourceObject[i];
		}

		//Return the raw vector data to the caller. Note that we've loaded the unpacked
		//data into the itemArray member, so it'll be freed when this object is destroyed.
		sourceData = itemArray;
		sourceDataLength = sourceObjectLength;
	}

private:
	mutable bool* itemArray;
	const std::vector<bool>& sourceObject;
};

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class STLObjectSource<std::array<ElementType, ArraySize>> :public ISTLObjectSource<std::array<ElementType, ArraySize>>
{
public:
	//Constructors
	STLObjectSource(const std::array<ElementType, ArraySize>& asourceObject)
		:sourceObject(asourceObject), itemArray(0), elementSizeArray(0), keyMarshallerArray(0)
	{ }
	virtual ~STLObjectSource()
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
	virtual void RetrieveData(const std::array<ElementType, ArraySize>*& sourceData) const
	{
		sourceData = sourceObject.data();
	}
	virtual void RetrieveData(const void*& aitemArray, const size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::array<ElementType, ArraySize>*)0);

		//Calculate the required size of our flat arrays in order to hold a decomposed
		//version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		CalculateDecomposedSTLContainerSize(nestingDepth, 1, itemArraySize, elementSizeArraySize, sourceObject);

		//Allocate our flat arrays to hold the decomposed object data
		delete[] itemArray;
		delete[] elementSizeArray;
		itemArray = CreateSTLContainerItemArray(itemArraySize, &sourceObject);
		elementSizeArray = new size_t[elementSizeArraySize];

		//Allocate key marshallers for any keyed containers in the source object
		if(HasKeyMarshallers())
		{
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

		//Decompose the container hierarchy into a flat array of the innermost elements,
		//with a corresponding array of sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t keyMarshallerArrayIndex = 0;
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, sourceObject);

		//Return the arrays of decomposed item data to the caller
		aitemArray = itemArray;
		aelementSizeArray = elementSizeArray;
		akeyMarshallerArray = keyMarshallerArray;
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
	const std::array<ElementType, ArraySize>& sourceObject;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
class STLObjectSource<std::basic_string<ElementType>> :public ISTLObjectSource<std::wstring>
{
public:
	//Constructors
	STLObjectSource(const std::basic_string<ElementType>& asourceObject)
		:sourceObject(asourceObject)
	{}

protected:
	//Marshalling methods
	virtual void RetrieveData(const ElementType*& sourceData, size_t& sourceDataLength) const
	{
		sourceData = sourceObject.data();
		sourceDataLength = sourceObject.size();
	}

private:
	const std::basic_string<ElementType>& sourceObject;
};

} //Close namespace InteropSupport
#endif
