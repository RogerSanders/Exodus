#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
#include "TupleHelper.h"
#endif
namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::vector<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::array<ElementType, ArraySize>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::list<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::forward_list<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::deque<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::set<ElementType, Compare, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::multiset<ElementType, Compare, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::stack<ElementType, Container>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::queue<ElementType, Container>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::priority_queue<ElementType, Container, Compare>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::basic_string<ElementType, traits, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class T1, class T2>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::pair<T1, T2>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	delete nestedMarshallerArray[nestedMarshallerArrayIndex+1];
}

#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
//----------------------------------------------------------------------------------------
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<>* elementPointer)
{ }

//----------------------------------------------------------------------------------------
template<class... Args>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer)
{
	TupleHelper<std::tuple_size<std::tuple<Args...>>::value-1, Args...>::DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex, elementPointer);
}

//----------------------------------------------------------------------------------------
template<size_t TupleElementNo, class... Args>
void TupleHelper<TupleElementNo, Args...>::DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
	TupleHelper<TupleElementNo-1, Args...>::DeleteSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, elementPointer);
}

//----------------------------------------------------------------------------------------
template<class... Args>
void TupleHelper<0, Args...>::DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer)
{
	delete nestedMarshallerArray[nestedMarshallerArrayIndex];
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const ElementType* elementPointer)
{ }

} //Close namespace Internal
} //Close namespace MarshalSupport
