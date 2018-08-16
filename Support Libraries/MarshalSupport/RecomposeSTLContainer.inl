#include "HasMarshalConstructor.h"
#include "INestedMarshaller.h"
#include "MarshalObjectHelper.h"
#include "IsThisOrNestedElementLastNestedContainerElement.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <memory.h>
#include <utility>
#endif
#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
#include "TupleHelper.h"
#endif
namespace MarshalSupport {
namespace Internal {

//Disable warning about our use of type traits causing conditional expressions to be constant. The code behaves as
//intended, and the compiler is free to optimize away the dead branch.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element)
{
	// We provide this namespace-level wrapper for our internal helper class here to allow this template to be
	// specialized from outside this library. We want users to be able to extend inbuilt support to include types not
	// included by default, however class definitions cannot be extended, so we can't have direct calls to specialized
	// member template functions within our library while keeping it extensible. In C++11 we could easily eliminate the
	// need for our helper class, and simply use a default template argument for IsLastElement instead, but we want to
	// also provide C++03 compatibility for at least the time being, and C++03 doesn't support default template
	// arguments for function templates. We provide this simple wrapper instead which makes our helper class an entirely
	// internal concern, and doesn't affect the ability to provide external specializations.
	RecomposeSTLContainerHelper<is_this_or_nested_element_last_nested_container_element<ElementType>::value, is_only_movable<typename get_last_nested_container_element_type<ElementType>::type>::value>::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, element);
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element[i]);
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>& element)
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

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true, true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
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

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, size_t ArraySize>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>& element)
{
	for(size_t i = 0; i < ArraySize; ++i)
	{
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element[i]);
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, size_t ArraySize>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>& element)
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

//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void RecomposeSTLContainerHelper<true, true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	for(size_t i = 0; i < ArraySize; ++i)
	{
		MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(*nextSourceObject), element[i]);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += ArraySize;
}
#endif

//----------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::list<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	typename std::list<ElementType, Alloc>::iterator containerIterator = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::list<ElementType, Alloc>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	typename std::forward_list<ElementType, Alloc>::iterator containerIterator = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::deque<ElementType, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	typename std::deque<ElementType, Alloc>::iterator containerIterator = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::deque<ElementType, Alloc>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		element.insert(MARSHALSUPPORT_MOVE(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		element.insert(MARSHALSUPPORT_MOVE(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		element.insert(std::move(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		element.insert(std::move(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	typename std::map<KeyType, ElementType, Compare, Alloc>::iterator insertPosition = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		insertPosition = MarshalObjectHelper<std::map<KeyType, ElementType, Compare, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(*nextSourceObject), element, insertPosition);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class KeyType, class ElementType, class Compare, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator insertPosition = element.begin();
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		insertPosition = MarshalObjectHelper<std::multimap<KeyType, ElementType, Compare, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), MARSHALSUPPORT_MOVE(*nextSourceObject), element, insertPosition);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		MarshalObjectHelper<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), std::move(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType containerEntry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, containerEntry);
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		element.insert(typename std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		INestedMarshaller<KeyType>* keyWrapper = (INestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
		MarshalObjectHelper<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>>::MarshalObjectToContainer(keyWrapper->RemoveKey(), std::move(*nextSourceObject), element);
		nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(nextSourceObject) + elementByteSize);
	}
	elementArrayIndex += containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::stack<ElementType, Container>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType entry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, entry);
		element.push(MARSHALSUPPORT_MOVE(entry));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::stack<ElementType, Container>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::queue<ElementType, Container>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType entry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, entry);
		element.push(MARSHALSUPPORT_MOVE(entry));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Container>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::queue<ElementType, Container>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class Container, class Compare>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		ElementType entry;
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, entry);
		element.push(MARSHALSUPPORT_MOVE(entry));
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class Container, class Compare>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element)
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
template<bool IsLastElement, bool IsOnlyMovable>
template<class ElementType, class traits, class Alloc>
void RecomposeSTLContainerHelper<IsLastElement, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	for(size_t i = 0; i < containerEntryCount; ++i)
	{
		::MarshalSupport::Internal::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element[i]);
	}
}

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType, class traits, class Alloc>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element)
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
template<bool IsOnlyMovable>
template<class T1, class T2>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::pair<T1, T2>& element)
{
	INestedMarshaller<T1>* keyWrapperT1 = (INestedMarshaller<T1>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	INestedMarshaller<T2>* keyWrapperT2 = (INestedMarshaller<T2>*)nestedMarshallerArray[nestedMarshallerArrayIndex+1];
	element.first = keyWrapperT1->RemoveKey();
	element.second = keyWrapperT2->RemoveKey();
}

#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<>& element)
{ }

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class... Args>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>& element)
{
	TupleHelper<std::tuple_size<std::tuple<Args...>>::value-1, Args...>::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, element);
}

//----------------------------------------------------------------------------------------
template<size_t TupleElementNo, class... Args>
void TupleHelper<TupleElementNo, Args...>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>& element)
{
	INestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>* keyWrapper = (INestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	std::get<TupleElementNo>(element) = std::move(keyWrapper->RemoveKey());
	TupleHelper<TupleElementNo-1, Args...>::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element);
}

//----------------------------------------------------------------------------------------
template<class... Args>
void TupleHelper<0, Args...>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>& element)
{
	INestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>* keyWrapper = (INestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	std::get<0>(element) = std::move(keyWrapper->RemoveKey());
}
#endif

//----------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
template<class ElementType>
void RecomposeSTLContainerHelper<true, IsOnlyMovable>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element)
{
	ElementType* nextSourceObject = reinterpret_cast<ElementType*>(reinterpret_cast<unsigned char*>(itemArray) + (elementArrayIndex * elementByteSize));
	MarshalObjectHelper<ElementType>::MarshalObjectToExistingObject(MARSHALSUPPORT_MOVE(*nextSourceObject), element);
	++elementArrayIndex;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainerHelper<true, true>::RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element)
{
	RecomposeSTLContainerHelper<true, false>::RecomposeSTLContainer(elementByteSize, itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, element);
}

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace Internal
} //Close namespace MarshalSupport
