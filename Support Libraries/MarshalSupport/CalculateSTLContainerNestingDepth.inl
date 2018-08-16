namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::vector<ElementType, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::array<ElementType, ArraySize>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::list<ElementType, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::forward_list<ElementType, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::deque<ElementType, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::set<ElementType, Compare, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::multiset<ElementType, Compare, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_set<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::map<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::multimap<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::stack<ElementType, Container>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::queue<ElementType, Container>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::priority_queue<ElementType, Container, Compare>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::basic_string<ElementType, traits, Alloc>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)0);
}

//----------------------------------------------------------------------------------------
template<class T1, class T2>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::pair<T1, T2>* elementPointer)
{
	return currentNestingDepth+2;
}

#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
//----------------------------------------------------------------------------------------
template<class... Args>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::tuple<Args...>* elementPointer)
{
	return currentNestingDepth+std::tuple_size<std::tuple<Args...>>::value;
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
MARSHALSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const ElementType* elementPointer)
{
	return currentNestingDepth;
}

} //Close namespace Internal
} //Close namespace MarshalSupport
