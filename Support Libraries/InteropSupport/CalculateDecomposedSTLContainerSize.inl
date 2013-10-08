namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::vector<ElementType>& element)
{
	++elementSizeArrayIndex;
	size_t containerEntryCount = element.size();
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += containerEntryCount;
	}
	else
	{
		for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, element[containerEntryNo]);
		}
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::array<ElementType, ArraySize>& element)
{
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += ArraySize;
	}
	else
	{
		for(size_t containerEntryNo = 0; containerEntryNo < ArraySize; ++containerEntryNo)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, element[containerEntryNo]);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::list<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::list<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, *containerIterator);
		}
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::forward_list<ElementType>& element)
{
	++elementSizeArrayIndex;
	for(std::forward_list<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
	{
		CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, *containerIterator);
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::deque<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::deque<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, *containerIterator);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::set<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::set<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, *containerIterator);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multiset<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::multiset<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, *containerIterator);
		}
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_set<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::unordered_set<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, *containerIterator);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multiset<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::unordered_multiset<ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, *containerIterator);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::map<KeyType, ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::map<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, containerIterator->second);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multimap<KeyType, ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::multimap<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, containerIterator->second);
		}
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_map<KeyType, ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::unordered_map<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, containerIterator->second);
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multimap<KeyType, ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		for(std::unordered_multimap<KeyType, ElementType>::const_iterator containerIterator = element.begin(); containerIterator != element.end(); ++containerIterator)
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, containerIterator->second);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::stack<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		std::stack<ElementType> elementCopy = element;
		while(!elementCopy.empty())
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, elementCopy.top());
			elementCopy.pop();
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::queue<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		std::queue<ElementType> elementCopy = element;
		while(!elementCopy.empty())
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, elementCopy.front());
			elementCopy.pop();
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::priority_queue<ElementType>& element)
{
	++elementSizeArrayIndex;
	if(currentNestingLevel >= nestingDepth)
	{
		elementArrayIndex += element.size();
	}
	else
	{
		std::priority_queue<ElementType> elementCopy = element;
		while(!elementCopy.empty())
		{
			CalculateDecomposedSTLContainerSize(nestingDepth, currentNestingLevel+1, elementArrayIndex, elementSizeArrayIndex, elementCopy.top());
			elementCopy.pop();
		}
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::basic_string<ElementType>& element)
{
	++elementSizeArrayIndex;
	size_t containerEntryCount = element.size();
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const ElementType& element)
{
	++elementArrayIndex;
}

} //Close namespace InteropSupport
