namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void ClearSTLContainer(std::vector<ElementType, Alloc>& element)
{
	element.clear();
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void ClearSTLContainer(std::array<ElementType, ArraySize>& element)
{ }
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void ClearSTLContainer(std::list<ElementType, Alloc>& element)
{
	element.clear();
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void ClearSTLContainer(std::forward_list<ElementType, Alloc>& element)
{
	element.clear();
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
void ClearSTLContainer(std::deque<ElementType, Alloc>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void ClearSTLContainer(std::set<ElementType, Compare, Alloc>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
void ClearSTLContainer(std::multiset<ElementType, Compare, Alloc>& element)
{
	element.clear();
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void ClearSTLContainer(std::unordered_set<ElementType, Hash, Pred, Alloc>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
void ClearSTLContainer(std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element)
{
	element.clear();
}
#endif

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void ClearSTLContainer(std::map<KeyType, ElementType, Compare, Alloc>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
void ClearSTLContainer(std::multimap<KeyType, ElementType, Compare, Alloc>& element)
{
	element.clear();
}

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void ClearSTLContainer(std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
void ClearSTLContainer(std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element)
{
	element.clear();
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void ClearSTLContainer(std::stack<ElementType, Container>& element)
{
	while(!element.empty())
	{
		element.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
void ClearSTLContainer(std::queue<ElementType, Container>& element)
{
	while(!element.empty())
	{
		element.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
void ClearSTLContainer(std::priority_queue<ElementType, Container, Compare>& element)
{
	while(!element.empty())
	{
		element.pop();
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
void ClearSTLContainer(std::basic_string<ElementType, traits, Alloc>& element)
{
	element.clear();
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void ClearSTLContainer(ElementType& element)
{ }

} //Close namespace Internal
} //Close namespace MarshalSupport
