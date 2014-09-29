#include "ISTLObjectKeyMarshaller.h"
#include "IMarshallingObject.h"
#include "MarshalObject.h"
#include <memory.h>
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <type_traits>
#endif
namespace InteropSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about our use of std::is_pod causing a conditional expression to be constant. The
//code behaves as intended, and the compiler is free to optimize away the dead branch.
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::vector<ElementType>& element)
{
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	if(std::is_pod<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), containerEntryCount * sizeof(ElementType));
		elementArrayIndex += containerEntryCount;
	}
	else
#endif
	{
		for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, element[containerEntryNo]);
		}
	}
}

//----------------------------------------------------------------------------------------
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::vector<bool>& element)
{
	//Note that we specifically exclude vectors from using memcpy here, since we can't do
	//that on the bit-packed specialization of a boolean vector that's still part of the
	//C++ standard at this time, and the data() method doesn't exist on this container
	//specialization either, so we can't use type traits within the generic vector
	//specialization to exclude it.
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
	bool* itemArrayWithType = (bool*)itemArray;
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		itemArrayWithType[elementArrayIndex++] = element[containerEntryNo];
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::array<ElementType, ArraySize>& element)
{
	if(std::is_pod<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), ArraySize * sizeof(ElementType));
		elementArrayIndex += ArraySize;
	}
	else
	{
		for(size_t containerEntryNo = 0; containerEntryNo < ArraySize; ++containerEntryNo)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, element[containerEntryNo]);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::list<ElementType>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for(std::list<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::forward_list<ElementType>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for(std::forward_list<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::deque<ElementType>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for(std::deque<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::set<ElementType>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for(std::set<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::multiset<ElementType>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for(std::multiset<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_set<ElementType>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for(std::unordered_set<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_multiset<ElementType>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for(std::unordered_multiset<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::map<KeyType, ElementType>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
	for(std::map<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		keyWrapper->AddKey(containerIterator->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerIterator->second);
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::multimap<KeyType, ElementType>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
	for(std::multimap<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		keyWrapper->AddKey(containerIterator->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerIterator->second);
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
	for(std::unordered_map<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		keyWrapper->AddKey(containerIterator->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerIterator->second);
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
	for(std::unordered_multimap<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		keyWrapper->AddKey(containerIterator->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerIterator->second);
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::stack<ElementType>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	std::stack<ElementType> elementCopy(element);
	std::vector<ElementType> elementContents(elementSize);
	for(size_t containerEntryNo = 0; containerEntryNo < elementSize; ++containerEntryNo)
	{
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		elementContents[containerEntryNo] = std::move(elementCopy.top());
#else
		elementContents[containerEntryNo] = elementCopy.top();
#endif
		elementCopy.pop();
	}
	for(size_t containerEntryNo = 0; containerEntryNo < elementSize; ++containerEntryNo)
	{
		size_t elementIndexNo = (elementSize - 1) - containerEntryNo;
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, elementContents[elementIndexNo]);
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::queue<ElementType>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	std::queue<ElementType> elementCopy(element);
	for(size_t containerEntryNo = 0; containerEntryNo < elementSize; ++containerEntryNo)
	{
		const ElementType& entry = elementCopy.front();
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
		elementCopy.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::priority_queue<ElementType>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	std::priority_queue<ElementType> elementCopy(element);
	for(size_t containerEntryNo = 0; containerEntryNo < elementSize; ++containerEntryNo)
	{
		const ElementType& entry = elementCopy.top();
		DecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
		elementCopy.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::basic_string<ElementType>& element)
{
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
	ElementType* itemArrayWithType = (ElementType*)itemArray;
	memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), containerEntryCount * sizeof(ElementType));
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const ElementType& element)
{
	ElementType* itemArrayWithType = (ElementType*)itemArray;
	ElementType& arrayElement = itemArrayWithType[elementArrayIndex++];
	if(!MarshalObject<INTEROPSUPPORT_ISBASEOF(IMarshallingObject, ElementType)>(&element, &arrayElement))
	{
		arrayElement = element;
	}
}

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace InteropSupport
