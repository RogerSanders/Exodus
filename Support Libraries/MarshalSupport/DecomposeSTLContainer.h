#ifndef __DECOMPOSESTLCONTAINER_H__
#define __DECOMPOSESTLCONTAINER_H__
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
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>&& element);
#endif
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::vector<bool>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::array<ElementType, ArraySize>& element);
template<class ElementType, size_t ArraySize>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>&& element);
#endif
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::list<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::list<ElementType, Alloc>&& element);
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::forward_list<ElementType, Alloc>& element);
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::forward_list<ElementType, Alloc>&& element);
#endif
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::deque<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::deque<ElementType, Alloc>&& element);
#endif
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::set<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>&& element);
#endif
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::multiset<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>&& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>&& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>&& element);
#endif
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>&& element);
#endif
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>&& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>&& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>&& element);
#endif
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::stack<ElementType, Container>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::stack<ElementType, Container>&& element);
#endif
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::queue<ElementType, Container>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::queue<ElementType, Container>&& element);
#endif
template<class ElementType, class Container, class Compare>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::priority_queue<ElementType, Container, Compare>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Container, class Compare>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>&& element);
#endif
template<class ElementType, class traits, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::basic_string<ElementType, traits, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class traits, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>&& element);
#endif
template<class T1, class T2>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::pair<T1, T2>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class T1, class T2>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::pair<T1, T2>&& element);
#endif
#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<>& element);
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<>&& element);
template<class... Args>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const std::tuple<Args...>& element);
template<class... Args>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>&& element);
#endif
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, const ElementType& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType&& element);
#endif

} // Close namespace Internal
} // Close namespace MarshalSupport
#include "DecomposeSTLContainer.inl"
#endif
