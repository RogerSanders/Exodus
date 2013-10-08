namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::vector<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::array<ElementType, ArraySize>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::list<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::forward_list<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::deque<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::set<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::multiset<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_set<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_multiset<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::map<KeyType, ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::multimap<KeyType, ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_map<KeyType, ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_multimap<KeyType, ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::stack<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::queue<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::priority_queue<ElementType>* elementPointer)
{
	return CalculateSTLContainerNestingDepth(currentNestingDepth+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::basic_string<ElementType>* elementPointer)
{
	return currentNestingDepth+1;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const ElementType* elementPointer)
{
	return currentNestingDepth;
}

} //Close namespace InteropSupport
