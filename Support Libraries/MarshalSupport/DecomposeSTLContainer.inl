#include "HasMarshalConstructor.h"
#include "INestedMarshaller.h"
#include "IsLastNestedContainerElement.h"
#include <memory.h>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <utility>
#endif
#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
#include "TupleHelper.h"
#endif
#include <cassert>
namespace MarshalSupport {
namespace Internal {

// Disable warning about our use of type traits causing conditional expressions to be constant. The code behaves as
// intended, and the compiler is free to optimize away the dead branch.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::vector<ElementType, Alloc>& element)
{
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	if (!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), containerEntryCount * sizeof(ElementType));
		elementArrayIndex += containerEntryCount;
	}
	else
#endif
	{
		for (size_t i = 0; i < containerEntryCount; ++i)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element[i]);
		}
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>&& element)
{
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
	if (!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), containerEntryCount * sizeof(ElementType));
		elementArrayIndex += containerEntryCount;
	}
	else
	{
		for (size_t i = 0; i < containerEntryCount; ++i)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(element[i]));
		}
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::vector<bool>& element)
{
	// Note that we specifically exclude vectors from using memcpy here, since we can't do
	// that on the bit-packed specialization of a boolean vector that's still part of the
	// C++ standard at this time, and the data() method doesn't exist on this container
	// specialization either, so we can't use type traits within the generic vector
	// specialization to exclude it.
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
	bool* itemArrayWithType = (bool*)itemArray;
	for (size_t i = 0; i < containerEntryCount; ++i)
	{
		itemArrayWithType[elementArrayIndex++] = element[i];
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::array<ElementType, ArraySize>& element)
{
	if (!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), ArraySize * sizeof(ElementType));
		elementArrayIndex += ArraySize;
	}
	else
	{
		for (size_t i = 0; i < ArraySize; ++i)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element[i]);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>&& element)
{
	if (!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), ArraySize * sizeof(ElementType));
		elementArrayIndex += ArraySize;
	}
	else
	{
		for (size_t i = 0; i < ArraySize; ++i)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(element[i]));
		}
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::list<ElementType, Alloc>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::list<ElementType, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::list<ElementType, Alloc>&& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::list<ElementType, Alloc>::iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(*i));
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::forward_list<ElementType, Alloc>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::forward_list<ElementType, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::forward_list<ElementType, Alloc>&& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::forward_list<ElementType, Alloc>::iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(*i));
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::deque<ElementType, Alloc>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::deque<ElementType, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::deque<ElementType, Alloc>&& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::deque<ElementType, Alloc>::iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(*i));
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::set<ElementType, Compare, Alloc>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::set<ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>&& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::set<ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		// Note that we can't use a non-const iterator and move the elements from this
		// container, because as per the C++ standard 23.2.4/6 iterator and const_iterator
		// are both const iterators for set containers. Casting away the const property and
		// performing a move here during iteration would cause undefined behaviour.
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::multiset<ElementType, Compare, Alloc>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::multiset<ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>&& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::multiset<ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		// Note that we can't use a non-const iterator and move the elements from this
		// container, because as per the C++ standard 23.2.4/6 iterator and const_iterator
		// are both const iterators for set containers. Casting away the const property and
		// performing a move here during iteration would cause undefined behaviour.
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::unordered_set<ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>&& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::unordered_set<ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		// Note that we can't use a non-const iterator and move the elements from this
		// container, because as per the C++ standard 23.2.4/6 iterator and const_iterator
		// are both const iterators for set containers. Casting away the const property and
		// performing a move here during iteration would cause undefined behaviour.
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::unordered_multiset<ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>&& element)
{
	size_t elementSizeArrayIndexForContainer = elementSizeArrayIndex++;
	size_t containerEntryCount = 0;
	for (typename std::unordered_multiset<ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		// Note that we can't use a non-const iterator and move the elements from this
		// container, because as per the C++ standard 23.2.4/6 iterator and const_iterator
		// are both const iterators for set containers. Casting away the const property and
		// performing a move here during iteration would cause undefined behaviour.
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, *i);
		++containerEntryCount;
	}
	elementSizeArray[elementSizeArrayIndexForContainer] = containerEntryCount;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::map<KeyType, ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, i->second);
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>&& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::map<KeyType, ElementType, Compare, Alloc>::iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(i->second));
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::multimap<KeyType, ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, i->second);
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>&& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(i->second));
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, i->second);
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>&& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>::iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(i->second));
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, i->second);
	}
}

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>&& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	NestedMarshaller<KeyType>* keyWrapper = (NestedMarshaller<KeyType>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	for (typename std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>::iterator i = element.begin(); i != element.end(); ++i)
	{
		keyWrapper->AddKey(i->first);
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(i->second));
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::stack<ElementType, Container>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	std::stack<ElementType, Container> elementCopy(element);
	std::vector<ElementType> elementContents;
	elementContents.reserve(elementSize);
	for (size_t i = 0; i < elementSize; ++i)
	{
		elementContents.push_back(MARSHALSUPPORT_MOVE(elementCopy.top()));
		elementCopy.pop();
	}
	for (size_t i = 0; i < elementSize; ++i)
	{
		size_t elementIndexNo = (elementSize - 1) - i;
		ElementType& entry = elementContents[elementIndexNo];
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, MARSHALSUPPORT_MOVE(entry));
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::stack<ElementType, Container>&& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	std::vector<ElementType> elementContents;
	elementContents.reserve(elementSize);
	for (size_t i = 0; i < elementSize; ++i)
	{
		elementContents.push_back(std::move(element.top()));
		element.pop();
	}
	for (size_t i = 0; i < elementSize; ++i)
	{
		size_t elementIndexNo = (elementSize - 1) - i;
		ElementType& entry = elementContents[elementIndexNo];
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(entry));
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::queue<ElementType, Container>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	std::queue<ElementType, Container> elementCopy(element);
	for (size_t i = 0; i < elementSize; ++i)
	{
		ElementType& entry = elementCopy.front();
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, MARSHALSUPPORT_MOVE(entry));
		elementCopy.pop();
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::queue<ElementType, Container>&& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	for (size_t i = 0; i < elementSize; ++i)
	{
		ElementType& entry = element.front();
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(entry));
		element.pop();
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::priority_queue<ElementType, Container, Compare>& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	std::priority_queue<ElementType, Container, Compare> elementCopy(element);
	for (size_t i = 0; i < elementSize; ++i)
	{
		// Note that the double copy here sucks, but due to an oversight in the current design of std::priority_queue,
		// there's no defined way to move an item off the queue. We could do a const cast here, and based on the very
		// specific way the behaviour of the std::priority_queue class is defined in the standard, that really should be
		// safe to do on any real-world implementation. We currently stick to the letter of the standard here however,
		// and perform another copy rather than expose us to any undefined behaviour.
		const ElementType& entry = elementCopy.top();
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, entry);
		elementCopy.pop();
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>&& element)
{
	size_t elementSize = element.size();
	elementSizeArray[elementSizeArrayIndex++] = elementSize;
	for (size_t i = 0; i < elementSize; ++i)
	{
		const ElementType& entry = element.top();
		DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, entry);
		element.pop();
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::basic_string<ElementType, traits, Alloc>& element)
{
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	if (!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), containerEntryCount * sizeof(ElementType));
		elementArrayIndex += containerEntryCount;
	}
	else
#endif
	{
		for (size_t i = 0; i < containerEntryCount; ++i)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element[i]);
		}
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>&& element)
{
	size_t containerEntryCount = element.size();
	elementSizeArray[elementSizeArrayIndex++] = containerEntryCount;
	if (!has_marshal_constructor<ElementType>::value && std::is_trivial<ElementType>::value)
	{
		ElementType* itemArrayWithType = (ElementType*)itemArray;
		memcpy((void*)&itemArrayWithType[elementArrayIndex], (const void*)element.data(), containerEntryCount * sizeof(ElementType));
		elementArrayIndex += containerEntryCount;
	}
	else
	{
		for (size_t i = 0; i < containerEntryCount; ++i)
		{
			DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(element[i]));
		}
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class T1, class T2>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::pair<T1, T2>& element)
{
	NestedMarshaller<T1>* keyWrapperT1 = (NestedMarshaller<T1>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	NestedMarshaller<T2>* keyWrapperT2 = (NestedMarshaller<T2>*)nestedMarshallerArray[nestedMarshallerArrayIndex+1];
	keyWrapperT1->AddKey(element.first);
	keyWrapperT2->AddKey(element.second);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class T1, class T2>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::pair<T1, T2>&& element)
{
	NestedMarshaller<T1>* keyWrapperT1 = (NestedMarshaller<T1>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	NestedMarshaller<T2>* keyWrapperT2 = (NestedMarshaller<T2>*)nestedMarshallerArray[nestedMarshallerArrayIndex+1];
	keyWrapperT1->AddKey(std::move(element.first));
	keyWrapperT2->AddKey(std::move(element.second));
}
#endif

#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
//----------------------------------------------------------------------------------------------------------------------
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<>& element)
{ }

//----------------------------------------------------------------------------------------------------------------------
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<>&& element)
{ }

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<Args...>& element)
{
	TupleHelper<std::tuple_size<std::tuple<Args...>>::value-1, Args...>::DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, element);
}

//----------------------------------------------------------------------------------------------------------------------
template<size_t TupleElementNo, class... Args>
void TupleHelper<TupleElementNo, Args...>::DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<Args...>& element)
{
	NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>* keyWrapper = (NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	keyWrapper->AddKey(std::get<TupleElementNo>(element));
	TupleHelper<TupleElementNo-1, Args...>::DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, element);
}

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
void TupleHelper<0, Args...>::DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<Args...>& element)
{
	NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>* keyWrapper = (NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	keyWrapper->AddKey(std::get<0>(element));
}

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>&& element)
{
	TupleHelper<std::tuple_size<std::tuple<Args...>>::value-1, Args...>::DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, std::move(element));
}

//----------------------------------------------------------------------------------------------------------------------
template<size_t TupleElementNo, class... Args>
void TupleHelper<TupleElementNo, Args...>::DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>&& element)
{
	NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>* keyWrapper = (NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	keyWrapper->AddKey(std::get<TupleElementNo>(std::move(element)));
	TupleHelper<TupleElementNo-1, Args...>::DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex+1, std::move(element));
}

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
void TupleHelper<0, Args...>::DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>&& element)
{
	NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>* keyWrapper = (NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>*)nestedMarshallerArray[nestedMarshallerArrayIndex];
	keyWrapper->AddKey(std::get<0>(std::move(element)));
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const ElementType& element)
{
	// Ensure that this function hasn't been called when there are still STL containers we need to decompose
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	static_assert(is_last_nested_container_element<ElementType>::value, "MarshalSupport internal error. DecomposeSTLContainer reached final element marshaller with a type that still contains nested elements.");
#else
	assert(is_last_nested_container_element<ElementType>::value);
#endif

	ElementType* itemArrayWithType = (ElementType*)itemArray;
	ElementType& arrayElement = itemArrayWithType[elementArrayIndex++];
	new((void*)&arrayElement) ElementType(element);
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element)
{
	// If a non-const lvalue reference has been supplied, convert it to a const lvalue reference and resolve the function
	// again. We only supply overloads on this function for STL container types using const lvalue and non-const rvalue
	// references, so we need to ensure that non-const lvalue references are made const before they are handled.
	DecomposeSTLContainer(itemArray, elementSizeArray, nestedMarshallerArray, elementArrayIndex, elementSizeArrayIndex, nestedMarshallerArrayIndex, (const ElementType&)element);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType>
void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType&& element)
{
	// This function takes a universal reference in. Note that this function should never be invoked with a const rvalue
	// reference. If this occurs, we have an internal error in our library. We use a static assertion to raise an error
	// in this case.
	static_assert(!std::is_const<ElementType>::value, "MarshalSupport internal error. DecomposeSTLContainer was called with a const rvalue reference.");

	// Ensure that this function hasn't been called when there are still STL containers we need to decompose
	static_assert(is_last_nested_container_element<typename std::decay<ElementType>::type>::value, "MarshalSupport internal error. DecomposeSTLContainer reached final element marshaller with a type that still contains nested elements.");

	auto* itemArrayWithType = (typename std::decay<ElementType>::type*)itemArray;
	auto& arrayElement = itemArrayWithType[elementArrayIndex++];
	new((void*)&arrayElement) typename std::decay<ElementType>::type(std::move(element));
}
#endif

// Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // Close namespace Internal
} // Close namespace MarshalSupport
