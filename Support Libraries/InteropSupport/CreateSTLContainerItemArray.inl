namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::vector<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::array<ElementType, ArraySize>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::list<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::forward_list<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::deque<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::set<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::multiset<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_set<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_multiset<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::map<KeyType, ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::multimap<KeyType, ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_map<KeyType, ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_multimap<KeyType, ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::stack<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::queue<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::priority_queue<ElementType>* elementPointer)
{
	return CreateSTLContainerItemArray(itemArraySize, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const std::basic_string<ElementType>* elementPointer)
{
	return (void*)new ElementType[itemArraySize];
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void* CreateSTLContainerItemArray(size_t itemArraySize, const ElementType* elementPointer)
{
	return (void*)new ElementType[itemArraySize];
}

} //Close namespace InteropSupport
