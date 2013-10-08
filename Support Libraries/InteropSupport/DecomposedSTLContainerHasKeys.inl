namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::vector<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::array<ElementType, ArraySize>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::list<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::forward_list<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::deque<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::set<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::multiset<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::unordered_set<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::unordered_multiset<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::map<KeyType, ElementType>* elementPointer)
{
	return true;
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::multimap<KeyType, ElementType>* elementPointer)
{
	return true;
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::unordered_map<KeyType, ElementType>* elementPointer)
{
	return true;
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::unordered_multimap<KeyType, ElementType>* elementPointer)
{
	return true;
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::stack<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::queue<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const std::priority_queue<ElementType>* elementPointer)
{
	return DecomposedSTLContainerHasKeys((const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR bool DecomposedSTLContainerHasKeys(const ElementType* elementPointer)
{
	return false;
}

} //Close namespace InteropSupport
