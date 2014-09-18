namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::vector<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void DeleteSTLContainerItemArray(void* itemArray, const std::array<ElementType, ArraySize>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::list<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::forward_list<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::deque<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::set<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::multiset<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::unordered_set<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::unordered_multiset<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerItemArray(void* itemArray, const std::map<KeyType, ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerItemArray(void* itemArray, const std::multimap<KeyType, ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerItemArray(void* itemArray, const std::unordered_map<KeyType, ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void DeleteSTLContainerItemArray(void* itemArray, const std::unordered_multimap<KeyType, ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::stack<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::queue<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::priority_queue<ElementType>* elementPointer)
{
	DeleteSTLContainerItemArray(itemArray, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const std::basic_string<ElementType>* elementPointer)
{
	ElementType* itemArrayAsElementTypeArray = (ElementType*)itemArray;
	delete[] itemArrayAsElementTypeArray;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void DeleteSTLContainerItemArray(void* itemArray, const ElementType* elementPointer)
{
	ElementType* itemArrayAsElementTypeArray = (ElementType*)itemArray;
	delete[] itemArrayAsElementTypeArray;
}

} //Close namespace InteropSupport
