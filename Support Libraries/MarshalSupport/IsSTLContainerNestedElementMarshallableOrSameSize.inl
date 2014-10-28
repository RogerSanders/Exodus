#include "HasMarshalConstructor.h"
namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::vector<ElementType, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::array<ElementType, ArraySize>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::list<ElementType, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::forward_list<ElementType, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::deque<ElementType, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::set<ElementType, Compare, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::multiset<ElementType, Compare, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::unordered_set<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::map<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::multimap<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::stack<ElementType, Container>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::queue<ElementType, Container>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::priority_queue<ElementType, Container, Compare>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const std::basic_string<ElementType, traits, Alloc>* elementPointer)
{
	return IsSTLContainerNestedElementMarshallableOrSameSize(expectedSize, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
MARSHALSUPPORT_CONSTEXPR bool IsSTLContainerNestedElementMarshallableOrSameSize(size_t expectedSize, const ElementType* elementPointer)
{
	return (has_marshal_constructor<ElementType>::value || (sizeof(ElementType) == expectedSize));
}

} //Close namespace Internal
} //Close namespace MarshalSupport
