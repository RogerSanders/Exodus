#ifndef __ISTLOBJECTSOURCE_H__
#define __ISTLOBJECTSOURCE_H__
#include "InteropPreprocessorMacros.h"
#include "CalculateSTLContainerNestingDepth.h"
#include "CalculateDecomposedSTLContainerSize.h"
#include "ClearSTLContainer.h"
#include "RecomposeSTLContainer.h"
#include "InteropPreprocessorMacros.h"
#include <string>
#include <vector>
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <array>
#include <type_traits>
#include <memory.h>
#endif
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ContainerType>
class ISTLObjectSource
{
public:
	//Constructors
	virtual ~ISTLObjectSource() = 0 {}

	//Marshalling methods
	ContainerType MarshalTo() const
	{
		ContainerType result;
		MarshalTo(result);
		return result;
	}
	void MarshalTo(ContainerType& targetObject) const
	{
		//Retrieve arrays to load the decomposed STL object data from
		const void* itemArray;
		const size_t* elementSizeArray;
		ISTLObjectKeyMarshallerBase* const* keyMarshallerArray;
		RetrieveData(itemArray, elementSizeArray, keyMarshallerArray);

		//Ensure the target container is cleared of any current data
		ClearSTLContainer(targetObject);

		//Recompose our flat arrays into the target container
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
	}

protected:
	//Marshalling methods
	virtual void RetrieveData(const void*& aitemArray, const size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const = 0;
};

//----------------------------------------------------------------------------------------
template<class ElementType>
class ISTLObjectSource<std::vector<ElementType>>
{
public:
	//Constructors
	virtual ~ISTLObjectSource() = 0 {}

	//Marshalling methods
	std::vector<ElementType> MarshalTo() const
	{
		std::vector<ElementType> result;
		MarshalTo(result);
		return result;
	}
	void MarshalTo(std::vector<ElementType>& targetObject) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::vector<ElementType>*)0);

		//If there are no nested STL containers within this container, marshal the
		//contents by retrieving a pointer to the first element in the contiguous array of
		//elements, otherwise decompose the source object into flat arrays and recompose
		//it into the target object.
		if(nestingDepth <= 1)
		{
			//Retrieve a pointer to the raw array contents in the target object
			const ElementType* sourceData;
			size_t sourceDataLength;
			RetrieveData(sourceData, sourceDataLength);

			//Copy the contents of the source container directly into the target object
			targetObject.assign(sourceData, sourceData + sourceDataLength);
		}
		else
		{
			//Retrieve arrays to load the decomposed STL object data from
			const void* itemArray;
			const size_t* elementSizeArray;
			ISTLObjectKeyMarshallerBase* const* keyMarshallerArray;
			RetrieveData(itemArray, elementSizeArray, keyMarshallerArray);

			//Recompose our flat arrays into the target container
			size_t elementArrayIndex = 0;
			size_t elementSizeArrayIndex = 0;
			RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
		}
	}

protected:
	//Marshalling methods
	virtual void RetrieveData(const ElementType*& sourceData, size_t& sourceDataLength) const = 0;
	virtual void RetrieveData(const void*& aitemArray, const size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const = 0;
};

//----------------------------------------------------------------------------------------
template<>
class ISTLObjectSource<std::vector<bool>>
{
public:
	//Constructors
	virtual ~ISTLObjectSource() = 0 {}

	//Marshalling methods
	std::vector<bool> MarshalTo() const
	{
		std::vector<bool> result;
		MarshalTo(result);
		return result;
	}
	void MarshalTo(std::vector<bool>& targetObject) const
	{
		//Retrieve a pointer to the raw array contents in the target object
		const bool* sourceData;
		size_t sourceDataLength;
		RetrieveData(sourceData, sourceDataLength);

		//Copy the contents of the source container directly into the target object
		targetObject.assign(sourceData, sourceData + sourceDataLength);
	}

protected:
	//Marshalling methods
	virtual void RetrieveData(const bool*& sourceData, size_t& sourceDataLength) const = 0;
};

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class ISTLObjectSource<std::array<ElementType, ArraySize>>
{
public:
	//Constructors
	virtual ~ISTLObjectSource() = 0 {}

	//Marshalling methods
	std::array<ElementType, ArraySize> MarshalTo() const
	{
		std::array<ElementType, ArraySize> result;
		MarshalTo(result);
		return result;
	}
	void MarshalTo(std::array<ElementType, ArraySize>& targetObject) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::array<ElementType, ArraySize>*)0);

		//If there are no nested STL containers within this container, marshal the
		//contents by retrieving a pointer to the first element in the contiguous array of
		//elements, otherwise decompose the source object into flat arrays and recompose
		//it into the target object.
		if(nestingDepth <= 1)
		{
			//Retrieve a pointer to the raw array contents in the target object
			const ElementType* sourceData;
			RetrieveData(sourceData);

			//Copy the contents of the source container directly into the target object
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
		else
		{
			//Retrieve arrays to load the decomposed STL object data from
			const void* itemArray;
			const size_t* elementSizeArray;
			ISTLObjectKeyMarshallerBase* const* keyMarshallerArray;
			RetrieveData(itemArray, elementSizeArray, keyMarshallerArray);

			//Recompose our flat arrays into the target container
			size_t elementArrayIndex = 0;
			size_t elementSizeArrayIndex = 0;
			RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, 0, targetObject);
		}
	}

protected:
	//Marshalling methods
	virtual void RetrieveData(const ElementType*& sourceData) const = 0;
	virtual void RetrieveData(const void*& aitemArray, const size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const = 0;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
class ISTLObjectSource<std::basic_string<ElementType>>
{
public:
	//Constructors
	virtual ~ISTLObjectSource() = 0 {}

	//Marshalling methods
	std::basic_string<ElementType> MarshalTo() const
	{
		std::basic_string<ElementType> result;
		MarshalTo(result);
		return result;
	}
	void MarshalTo(std::basic_string<ElementType>& targetObject) const
	{
		//Retrieve the data from the source object
		const ElementType* sourceData;
		size_t sourceDataLength;
		RetrieveData(sourceData, sourceDataLength);

		//Directly load the source data into the target object
		targetObject.assign(sourceData, sourceDataLength);
	}

protected:
	//Marshalling methods
	virtual void RetrieveData(const ElementType*& sourceData, size_t& sourceDataLength) const = 0;
};

} //Close namespace InteropSupport
#endif
