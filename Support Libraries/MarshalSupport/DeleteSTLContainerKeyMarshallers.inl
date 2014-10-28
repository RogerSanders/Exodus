namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::vector<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::array<ElementType, ArraySize>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::list<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::forward_list<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::deque<ElementType, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::set<ElementType, Compare, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multiset<ElementType, Compare, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::stack<ElementType, Container>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::queue<ElementType, Container>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::priority_queue<ElementType, Container, Compare>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::basic_string<ElementType, traits, Alloc>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const ElementType* elementPointer)
{ }

} //Close namespace Internal
} //Close namespace MarshalSupport
