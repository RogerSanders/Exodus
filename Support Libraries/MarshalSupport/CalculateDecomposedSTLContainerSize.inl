#include "IsLastNestedContainerElement.h"
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
template<class ElementType, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::vector<ElementType, Alloc>& element)
{
	++elementSizeArrayIndex;
	size_t containerEntryCount = element.size();
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += containerEntryCount;
	}
	else
	{
		for(size_t i = 0; i < containerEntryCount; ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, element[i]);
		}
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::array<ElementType, ArraySize>& element)
{
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += ArraySize;
	}
	else
	{
		for(size_t i = 0; i < ArraySize; ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, element[i]);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::list<ElementType, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::list<ElementType, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, *i);
		}
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::forward_list<ElementType, Alloc>& element)
{
	++elementSizeArrayIndex;
	for(typename std::forward_list<ElementType, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
	{
		CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, *i);
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::deque<ElementType, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::deque<ElementType, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, *i);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::set<ElementType, Compare, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::set<ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, *i);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multiset<ElementType, Compare, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::multiset<ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, *i);
		}
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::unordered_set<ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, *i);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::unordered_multiset<ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, *i);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::map<KeyType, ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, i->second);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::multimap<KeyType, ElementType, Compare, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, i->second);
		}
	}
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, i->second);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(typename std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>::const_iterator i = element.begin(); i != element.end(); ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, i->second);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::stack<ElementType, Container>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		std::stack<ElementType, Container> elementCopy = element;
		while(!elementCopy.empty())
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, elementCopy.top());
			elementCopy.pop();
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::queue<ElementType, Container>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		std::queue<ElementType, Container> elementCopy = element;
		while(!elementCopy.empty())
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, elementCopy.front());
			elementCopy.pop();
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::priority_queue<ElementType, Container, Compare>& element)
{
	++elementSizeArrayIndex;
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		std::priority_queue<ElementType, Container, Compare> elementCopy = element;
		while(!elementCopy.empty())
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, elementCopy.top());
			elementCopy.pop();
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::basic_string<ElementType, traits, Alloc>& element)
{
	++elementSizeArrayIndex;
	size_t containerEntryCount = element.size();
	if(is_last_nested_container_element<ElementType>::value)
	{
		elementArrayIndex += containerEntryCount;
	}
	else
	{
		for(size_t i = 0; i < containerEntryCount; ++i)
		{
			CalculateDecomposedSTLContainerSize(elementArrayIndex, elementSizeArrayIndex, element[i]);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const ElementType& element)
{
	++elementArrayIndex;
}

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace Internal
} //Close namespace MarshalSupport
