#ifndef __ISLASTNESTEDCONTAINERELEMENT_H__
#define __ISLASTNESTEDCONTAINERELEMENT_H__
#include "MarshalPreprocessorMacros.h"
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#endif
namespace MarshalSupport {
namespace Internal {

template<class T>
struct is_last_nested_container_element
{
private:
	//Typedefs
	typedef char(&yes)[1];
	typedef char(&no)[2];

private:
	//Check function
	template<class ElementType, class Alloc>
	static no check(std::vector<ElementType, Alloc>*);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, size_t ArraySize>
	static no check(std::array<ElementType, ArraySize>*);
#endif
	template<class ElementType, class Alloc>
	static no check(std::list<ElementType, Alloc>*);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Alloc>
	static no check(std::forward_list<ElementType, Alloc>*);
#endif
	template<class ElementType, class Alloc>
	static no check(std::deque<ElementType, Alloc>*);
	template<class ElementType, class Compare, class Alloc>
	static no check(std::set<ElementType, Compare, Alloc>*);
	template<class ElementType, class Compare, class Alloc>
	static no check(std::multiset<ElementType, Compare, Alloc>*);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Hash, class Pred, class Alloc>
	static no check(std::unordered_set<ElementType, Hash, Pred, Alloc>*);
	template<class ElementType, class Hash, class Pred, class Alloc>
	static no check(std::unordered_multiset<ElementType, Hash, Pred, Alloc>*);
#endif
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static no check(std::map<KeyType, ElementType, Compare, Alloc>*);
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static no check(std::multimap<KeyType, ElementType, Compare, Alloc>*);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static no check(std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>*);
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static no check(std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>*);
#endif
	template<class ElementType, class Container>
	static no check(std::stack<ElementType, Container>*);
	template<class ElementType, class Container>
	static no check(std::queue<ElementType, Container>*);
	template<class ElementType, class Container, class Compare>
	static no check(std::priority_queue<ElementType, Container, Compare>*);
	template<class ElementType, class traits, class Alloc>
	static no check(std::basic_string<ElementType, traits, Alloc>*);
	template<class ElementType>
	static yes check(ElementType*);

public:
	//Result
	static const bool value = (sizeof(check((T*)0)) == sizeof(yes));
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
