namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::vector<ElementType>& element)
{
	element.clear();
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void ClearSTLContainer(std::array<ElementType, ArraySize>& element)
{ }
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::list<ElementType>& element)
{
	element.clear();
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::forward_list<ElementType>& element)
{
	element.clear();
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::deque<ElementType>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::set<ElementType>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::multiset<ElementType>& element)
{
	element.clear();
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::unordered_set<ElementType>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::unordered_multiset<ElementType>& element)
{
	element.clear();
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void ClearSTLContainer(std::map<KeyType, ElementType>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void ClearSTLContainer(std::multimap<KeyType, ElementType>& element)
{
	element.clear();
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void ClearSTLContainer(std::unordered_map<KeyType, ElementType>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void ClearSTLContainer(std::unordered_multimap<KeyType, ElementType>& element)
{
	element.clear();
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::stack<ElementType>& element)
{
	while(!element.empty())
	{
		element.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::queue<ElementType>& element)
{
	while(!element.empty())
	{
		element.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::priority_queue<ElementType>& element)
{
	while(!element.empty())
	{
		element.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(std::basic_string<ElementType>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(ElementType& element)
{ }

} //Close namespace InteropSupport
