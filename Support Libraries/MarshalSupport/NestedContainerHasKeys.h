#ifndef __NESTEDCONTAINERHASKEYS_H__
#define __NESTEDCONTAINERHASKEYS_H__
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

template<class T> struct nested_container_has_keys { static const bool value = false; };
template<class ElementType, class Alloc> struct nested_container_has_keys<std::vector<ElementType, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize> struct nested_container_has_keys<std::array<ElementType, ArraySize>> { static const bool value = nested_container_has_keys<ElementType>::value; };
#endif
template<class ElementType, class Alloc> struct nested_container_has_keys<std::list<ElementType, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc> struct nested_container_has_keys<std::forward_list<ElementType, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
#endif
template<class ElementType, class Alloc> struct nested_container_has_keys<std::deque<ElementType, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
template<class ElementType, class Compare, class Alloc> struct nested_container_has_keys<std::set<ElementType, Compare, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
template<class ElementType, class Compare, class Alloc> struct nested_container_has_keys<std::multiset<ElementType, Compare, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Hash, class Pred, class Alloc> struct nested_container_has_keys<std::unordered_set<ElementType, Hash, Pred, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
template<class ElementType, class Hash, class Pred, class Alloc> struct nested_container_has_keys<std::unordered_multiset<ElementType, Hash, Pred, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };
#endif
template<class KeyType, class ElementType, class Compare, class Alloc> struct nested_container_has_keys<std::map<KeyType, ElementType, Compare, Alloc>> { static const bool value = true; };
template<class KeyType, class ElementType, class Compare, class Alloc> struct nested_container_has_keys<std::multimap<KeyType, ElementType, Compare, Alloc>> { static const bool value = true; };
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc> struct nested_container_has_keys<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>> { static const bool value = true; };
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc> struct nested_container_has_keys<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>> { static const bool value = true; };
#endif
template<class ElementType, class Container> struct nested_container_has_keys<std::stack<ElementType, Container>> { static const bool value = nested_container_has_keys<ElementType>::value; };
template<class ElementType, class Container> struct nested_container_has_keys<std::queue<ElementType, Container>> { static const bool value = nested_container_has_keys<ElementType>::value; };
template<class ElementType, class Container, class Compare> struct nested_container_has_keys<std::priority_queue<ElementType, Container, Compare>> { static const bool value = nested_container_has_keys<ElementType>::value; };
template<class ElementType, class traits, class Alloc> struct nested_container_has_keys<std::basic_string<ElementType, traits, Alloc>> { static const bool value = nested_container_has_keys<ElementType>::value; };

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
