namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::vector<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::array<ElementType, ArraySize>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::list<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::forward_list<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::deque<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::set<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multiset<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_set<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multiset<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::map<KeyType, ElementType>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multimap<KeyType, ElementType>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::stack<ElementType>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::queue<ElementType>* elementPointer)
{
	delete keyMarshallerArray[keyMarshallerArrayIndex];
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::priority_queue<ElementType>* elementPointer)
{
	DeleteSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const ElementType* elementPointer)
{ }

} //Close namespace InteropSupport
