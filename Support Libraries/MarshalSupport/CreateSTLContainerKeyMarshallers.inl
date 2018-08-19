#include "NestedMarshaller.h"
#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
#include "TupleHelper.h"
#endif
namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::vector<ElementType, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::array<ElementType, ArraySize>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::list<ElementType, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::forward_list<ElementType, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::deque<ElementType, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::set<ElementType, Compare, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::multiset<ElementType, Compare, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	nestedMarshallerArray[nestedMarshallerArrayIndex] = new NestedMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	nestedMarshallerArray[nestedMarshallerArrayIndex] = new NestedMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	nestedMarshallerArray[nestedMarshallerArrayIndex] = new NestedMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	nestedMarshallerArray[nestedMarshallerArrayIndex] = new NestedMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::stack<ElementType, Container>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::queue<ElementType, Container>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::priority_queue<ElementType, Container, Compare>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::basic_string<ElementType, traits, Alloc>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class T1, class T2>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::pair<T1, T2>* elementPointer)
{
	nestedMarshallerArray[nestedMarshallerArrayIndex] = new NestedMarshaller<T1>();
	nestedMarshallerArray[nestedMarshallerArrayIndex+1] = new NestedMarshaller<T2>();
}

#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
//----------------------------------------------------------------------------------------------------------------------
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<>* elementPointer)
{ }

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer)
{
	TupleHelper<std::tuple_size<std::tuple<Args...>>::value-1, Args...>::CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex, elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<size_t TupleElementNo, class... Args>
void TupleHelper<TupleElementNo, Args...>::CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer)
{
	nestedMarshallerArray[nestedMarshallerArrayIndex] = new NestedMarshaller<typename std::tuple_element<TupleElementNo, std::tuple<Args...>>::type>();
	TupleHelper<TupleElementNo-1, Args...>::CreateSTLContainerKeyMarshallers(nestedMarshallerArray, nestedMarshallerArrayIndex+1, elementPointer);
}

//----------------------------------------------------------------------------------------------------------------------
template<class... Args>
void TupleHelper<0, Args...>::CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer)
{
	nestedMarshallerArray[nestedMarshallerArrayIndex] = new NestedMarshaller<typename std::tuple_element<0, std::tuple<Args...>>::type>();
}
#endif

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const ElementType* elementPointer)
{ }

} // Close namespace Internal
} // Close namespace MarshalSupport
