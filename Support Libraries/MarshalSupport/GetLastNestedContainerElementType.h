#ifndef __GETLASTNESTEDCONTAINERELEMENTTYPE_H__
#define __GETLASTNESTEDCONTAINERELEMENTTYPE_H__
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

template<class ElementType> struct get_last_nested_container_element_type { typedef ElementType type; };
template<class ElementType, class Alloc> struct get_last_nested_container_element_type<std::vector<ElementType, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize> struct get_last_nested_container_element_type<std::array<ElementType, ArraySize>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#endif
template<class ElementType, class Alloc> struct get_last_nested_container_element_type<std::list<ElementType, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc> struct get_last_nested_container_element_type<std::forward_list<ElementType, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#endif
template<class ElementType, class Alloc> struct get_last_nested_container_element_type<std::deque<ElementType, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class ElementType, class Compare, class Alloc> struct get_last_nested_container_element_type<std::set<ElementType, Compare, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class ElementType, class Compare, class Alloc> struct get_last_nested_container_element_type<std::multiset<ElementType, Compare, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Hash, class Pred, class Alloc> struct get_last_nested_container_element_type<std::unordered_set<ElementType, Hash, Pred, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class ElementType, class Hash, class Pred, class Alloc> struct get_last_nested_container_element_type<std::unordered_multiset<ElementType, Hash, Pred, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#endif
template<class KeyType, class ElementType, class Compare, class Alloc> struct get_last_nested_container_element_type<std::map<KeyType, ElementType, Compare, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class KeyType, class ElementType, class Compare, class Alloc> struct get_last_nested_container_element_type<std::multimap<KeyType, ElementType, Compare, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc> struct get_last_nested_container_element_type<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc> struct get_last_nested_container_element_type<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
#endif
template<class ElementType, class Container> struct get_last_nested_container_element_type<std::stack<ElementType, Container>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class ElementType, class Container> struct get_last_nested_container_element_type<std::queue<ElementType, Container>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class ElementType, class Container, class Compare> struct get_last_nested_container_element_type<std::priority_queue<ElementType, Container, Compare>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };
template<class ElementType, class traits, class Alloc> struct get_last_nested_container_element_type<std::basic_string<ElementType, traits, Alloc>> { typedef typename get_last_nested_container_element_type<ElementType>::type type; };

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
