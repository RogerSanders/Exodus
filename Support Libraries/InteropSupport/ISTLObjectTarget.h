#ifndef __ISTLOBJECTTARGET_H__
#define __ISTLOBJECTTARGET_H__
#include "InteropPreprocessorMacros.h"
#include "CalculateSTLContainerNestingDepth.h"
#include "CalculateDecomposedSTLContainerSize.h"
#include "DecomposeSTLContainer.h"
#include "InteropPreprocessorMacros.h"
#include <string>
#include <vector>
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <array>
#endif
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ContainerType>
class ISTLObjectTarget
{
public:
	//Constructors
	virtual ~ISTLObjectTarget() = 0 {}

	//Marshalling methods
	void MarshalFrom(const ContainerType& sourceObject) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (ContainerType*)0);

		//Calculate the required size of our flat arrays in order to hold a decomposed
		//version of the specified object
		size_t itemArraySize = 0;
		size_t elementSizeArraySize = 0;
		CalculateDecomposedSTLContainerSize(nestingDepth, 1, itemArraySize, elementSizeArraySize, sourceObject);

		//Retrieve arrays to load the decomposed STL object data into, specifying the
		//required sizes for the item array and element size array.
		void* itemArray;
		size_t* elementSizeArray;
		ISTLObjectKeyMarshallerBase* const* keyMarshallerArray;
		RetrieveFlatArrays(itemArraySize, elementSizeArraySize, itemArray, elementSizeArray, keyMarshallerArray);

		//Decompose the container hierarchy into a flat array of the innermost elements,
		//with a corresponding array of sizes for the intermediate containers.
		size_t elementArrayIndex = 0;
		size_t elementSizeArrayIndex = 0;
		size_t keyMarshallerArrayIndex = 0;
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, sourceObject);

		//Recompose our flat arrays into the target container
		MarshalFromFlatArrays();
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const = 0;
	virtual void MarshalFromFlatArrays() const = 0;
};

//----------------------------------------------------------------------------------------
template<class ElementType>
class ISTLObjectTarget<std::vector<ElementType>>
{
public:
	//Constructors
	virtual ~ISTLObjectTarget() = 0 {}

	//Marshalling methods
	void MarshalFrom(const std::vector<ElementType>& sourceObject) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::vector<ElementType>*)0);

		//If there are no nested STL containers within this container, marshal the
		//contents by sending a pointer to the first element in the contiguous array of
		//elements, otherwise decompose the source object into flat arrays and recompose
		//it into the target object.
		if(nestingDepth <= 1)
		{
			//Marshal the contents of this container directly into the target object
			const ElementType* sourceData = sourceObject.empty()? 0: &sourceObject[0];
			MarshalFrom(sourceData, sourceObject.size());
		}
		else
		{
			//Calculate the required size of our flat arrays in order to hold a decomposed
			//version of the specified object
			size_t itemArraySize = 0;
			size_t elementSizeArraySize = 0;
			CalculateDecomposedSTLContainerSize(nestingDepth, 1, itemArraySize, elementSizeArraySize, sourceObject);

			//Retrieve arrays to load the decomposed STL object data into, specifying the
			//required sizes for the item array and element size array.
			void* itemArray;
			size_t* elementSizeArray;
			ISTLObjectKeyMarshallerBase* const* keyMarshallerArray;
			RetrieveFlatArrays(itemArraySize, elementSizeArraySize, itemArray, elementSizeArray, keyMarshallerArray);

			//Decompose the container hierarchy into a flat array of the innermost
			//elements, with a corresponding array of sizes for the intermediate
			//containers.
			size_t elementArrayIndex = 0;
			size_t elementSizeArrayIndex = 0;
			size_t keyMarshallerArrayIndex = 0;
			DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, sourceObject);

			//Recompose our flat arrays into the target container
			MarshalFromFlatArrays();
		}
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const = 0;
	virtual void MarshalFrom(const ElementType* sourceData, size_t sourceDataLength) const = 0;
	virtual void MarshalFromFlatArrays() const = 0;
};

//----------------------------------------------------------------------------------------
template<>
class ISTLObjectTarget<std::vector<bool>>
{
public:
	//Constructors
	virtual ~ISTLObjectTarget() = 0 {}

	//Marshalling methods
	void MarshalFrom(const std::vector<bool>& sourceObject) const
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

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
class ISTLObjectTarget<std::array<ElementType, ArraySize>>
{
public:
	//Constructors
	virtual ~ISTLObjectTarget() = 0 {}

	//Marshalling methods
	void MarshalFrom(const std::array<ElementType, ArraySize>& sourceObject) const
	{
		//Determine the number of STL container nesting levels present in the target
		//object
		INTEROPSUPPORT_CONSTEXPR size_t nestingDepth = CalculateSTLContainerNestingDepth(0, (std::array<ElementType, size_t ArraySize>*)0);

		//If there are no nested STL containers within this container, marshal the
		//contents by sending a pointer to the first element in the contiguous array of
		//elements, otherwise decompose the source object into flat arrays and recompose
		//it into the target object.
		if(nestingDepth <= 1)
		{
			//Marshal the contents of this container directly into the target object
			const ElementType* sourceData = sourceObject.empty()? 0: &sourceObject[0];
			MarshalFrom(sourceData);
		}
		else
		{
			//Calculate the required size of our flat arrays in order to hold a decomposed
			//version of the specified object
			size_t itemArraySize = 0;
			size_t elementSizeArraySize = 0;
			CalculateDecomposedSTLContainerSize(nestingDepth, 1, itemArraySize, elementSizeArraySize, sourceObject);

			//Retrieve arrays to load the decomposed STL object data into, specifying the
			//required sizes for the item array and element size array.
			void* itemArray;
			size_t* elementSizeArray;
			ISTLObjectKeyMarshallerBase* const* keyMarshallerArray;
			RetrieveFlatArrays(itemArraySize, elementSizeArraySize, itemArray, elementSizeArray, keyMarshallerArray);

			//Decompose the container hierarchy into a flat array of the innermost
			//elements, with a corresponding array of sizes for the intermediate
			//containers.
			size_t elementArrayIndex = 0;
			size_t elementSizeArrayIndex = 0;
			size_t keyMarshallerArrayIndex = 0;
			DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex, sourceObject);

			//Recompose our flat arrays into the target container
			MarshalFromFlatArrays();
		}
	}

protected:
	//Marshalling methods
	virtual void RetrieveFlatArrays(size_t itemArraySize, size_t elementSizeArraySize, void*& aitemArray, size_t*& aelementSizeArray, ISTLObjectKeyMarshallerBase* const*& akeyMarshallerArray) const = 0;
	virtual void MarshalFrom(const ElementType* sourceData) const = 0;
	virtual void MarshalFromFlatArrays() const = 0;
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
class ISTLObjectTarget<std::basic_string<ElementType>>
{
public:
	//Constructors
	virtual ~ISTLObjectTarget() = 0 {}

	//Marshalling methods
	void MarshalFrom(const std::basic_string<ElementType>& sourceObject) const
	{
		MarshalFrom(sourceObject.data(), sourceObject.size());
	}

protected:
	//Marshalling methods
	virtual void MarshalFrom(const ElementType* sourceData, size_t sourceDataLength) const = 0;
};

} //Close namespace InteropSupport
#endif
