#ifndef __DELETESTLCONTAINERKEYMARSHALLERS_H__
#define __DELETESTLCONTAINERKEYMARSHALLERS_H__
#include "MarshalPreprocessorMacros.h"
#include "INestedMarshallerBase.h"
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include <utility>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#endif
namespace MarshalSupport {
namespace Internal {

template<class ElementType, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::vector<ElementType, Alloc>* elementPointer);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::array<ElementType, ArraySize>* elementPointer);
#endif
template<class ElementType, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::list<ElementType, Alloc>* elementPointer);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::forward_list<ElementType, Alloc>* elementPointer);
#endif
template<class ElementType, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::deque<ElementType, Alloc>* elementPointer);
template<class ElementType, class Compare, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::set<ElementType, Compare, Alloc>* elementPointer);
template<class ElementType, class Compare, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::multiset<ElementType, Compare, Alloc>* elementPointer);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>* elementPointer);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>* elementPointer);
#endif
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>* elementPointer);
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>* elementPointer);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>* elementPointer);
#endif
template<class ElementType, class Container>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::stack<ElementType, Container>* elementPointer);
template<class ElementType, class Container>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::queue<ElementType, Container>* elementPointer);
template<class ElementType, class Container, class Compare>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::priority_queue<ElementType, Container, Compare>* elementPointer);
template<class ElementType, class traits, class Alloc>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::basic_string<ElementType, traits, Alloc>* elementPointer);
template<class T1, class T2>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::pair<T1, T2>* elementPointer);
#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<>* elementPointer);
template<class... Args>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const std::tuple<Args...>* elementPointer);
#endif
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(INestedMarshallerBase* nestedMarshallerArray[], size_t nestedMarshallerArrayIndex, const ElementType* elementPointer);

} //Close namespace Internal
} //Close namespace MarshalSupport
#include "DeleteSTLContainerKeyMarshallers.inl"
#endif
