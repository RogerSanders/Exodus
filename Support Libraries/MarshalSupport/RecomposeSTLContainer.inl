#include "HasMarshalConstructor.h"
#include "ISTLObjectKeyMarshaller.h"
#include "MarshalObjectHelper.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <memory.h>
#include <utility>
#endif
namespace MarshalSupport {
namespace Internal {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about our use of type traits causing conditional expressions to be
//constant. The code behaves as intended, and the compiler is free to optimize away the
//dead branch.
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::vector<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, element[i]);
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::vector<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	if(!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		element.assign(itemArrayWithType + elementArrayIndex, itemArrayWithType + elementArrayIndex + containerEntryCount);
		elementArrayIndex += containerEntryCount;
	}
	else
#endif
	{
		ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
		element.clear();
		element.reserve(containerEntryCount);
		for(size_t i = 0; i < containerEntryCount; ++i)
		{
			MarshalObjectHelper<std::vector<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
			nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
		}
		elementArrayIndex += containerEntryCount;
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, size_t ArraySize>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::array<ElementType, ArraySize>& element)
{
	for(size_t i = 0; i < ArraySize; ++i)
	{
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, element[i]);
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::array<ElementType, ArraySize>& element)
{
	if(!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)element.data(), (const void*)&itemArrayWithType[elementArrayIndex], ArraySize * sizeof(ElementType));
		elementArrayIndex += ArraySize;
	}
	else
	{
		ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
		for(size_t i = 0; i < ArraySize; ++i)
		{
			MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(*nextSourceObject), element[i]);
			nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
		}
		elementArrayIndex += ArraySize;
	}
}
#endif

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::list<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	typename std::list<ElementType, Alloc>::iterator containerIterator = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::list<ElementType, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		MarshalObjectHelper<std::list<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	typename std::forward_list<ElementType, Alloc>::iterator containerIterator = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	typename std::forward_list<ElementType, Alloc>::iterator insertPosition = element.before_begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		insertPosition = MarshalObjectHelper<std::forward_list<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element, insertPosition);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::deque<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	typename std::deque<ElementType, Alloc>::iterator containerIterator = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::deque<ElementType, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		MarshalObjectHelper<std::deque<ElementType, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		element.insert(MARSHALSUPPORT_MOVE(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	typename std::set<ElementType, Compare, Alloc>::iterator insertPosition = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		insertPosition = MarshalObjectHelper<std::set<ElementType, Compare, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element, insertPosition);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		element.insert(MARSHALSUPPORT_MOVE(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	typename std::multiset<ElementType, Compare, Alloc>::iterator insertPosition = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		insertPosition = MarshalObjectHelper<std::multiset<ElementType, Compare, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element, insertPosition);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		element.insert(std::move(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		MarshalObjectHelper<std::unordered_set<ElementType, Hash, Pred, Alloc>>::MarshalObjectToContainer(std::move(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		element.insert(std::move(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		MarshalObjectHelper<std::unordered_multiset<ElementType, Hash, Pred, Alloc>>::MarshalObjectToContainer(std::move(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	typename std::map<KeyType, ElementType, Compare, Alloc>::iterator insertPosition = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		insertPosition = MarshalObjectHelper<std::map<KeyType, ElementType, Compare, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(*nextSourceObject), element, insertPosition);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator insertPosition = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		insertPosition = MarshalObjectHelper<std::multimap<KeyType, ElementType, Compare, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(*nextSourceObject), element, insertPosition);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		MarshalObjectHelper<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), std::move(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		MarshalObjectHelper<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), std::move(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::stack<ElementType, Container>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType entry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
		element.push(MARSHALSUPPORT_MOVE(entry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::stack<ElementType, Container>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		MarshalObjectHelper<std::stack<ElementType, Container>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::queue<ElementType, Container>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType entry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
		element.push(MARSHALSUPPORT_MOVE(entry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::queue<ElementType, Container>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		MarshalObjectHelper<std::queue<ElementType, Container>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class Container, class Compare>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType entry;
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
		element.push(MARSHALSUPPORT_MOVE(entry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		MarshalObjectHelper<std::priority_queue<ElementType, Container, Compare>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
template<class ElementType, class traits, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value>::template RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, element[i]);
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	if(!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		element.assign(itemArrayWithType + elementArrayIndex, itemArrayWithType + elementArrayIndex + containerEntryCount);
		elementArrayIndex += containerEntryCount;
	}
	else
#endif
	{
		ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
		element.clear();
		element.reserve(containerEntryCount);
		for(size_t i = 0; i < containerEntryCount; ++i)
		{
			MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>>::MarshalObjectToContainer(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
			nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
		}
		elementArrayIndex += containerEntryCount;
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainerHelper<true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, ElementType& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
	++elementArrayIndex;
}

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace Internal
} //Close namespace MarshalSupport
