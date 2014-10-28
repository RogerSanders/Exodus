#ifndef __CALCULATEDECOMPOSEDSTLCONTAINERSIZE_H__
#define __CALCULATEDECOMPOSEDSTLCONTAINERSIZE_H__
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

template<class ElementType, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::array<ElementType, ArraySize>& element);
#endif
template<class ElementType, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::list<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::forward_list<ElementType, Alloc>& element);
#endif
template<class ElementType, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::deque<ElementType, Alloc>& element);
template<class ElementType, class Compare, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::set<ElementType, Compare, Alloc>& element);
template<class ElementType, class Compare, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multiset<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Hash, class Pred, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element);
#endif
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>& element);
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element);
#endif
template<class ElementType, class Container>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::stack<ElementType, Container>& element);
template<class ElementType, class Container>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::queue<ElementType, Container>& element);
template<class ElementType, class Container, class Compare>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::priority_queue<ElementType, Container, Compare>& element);
template<class ElementType, class traits, class Alloc>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::basic_string<ElementType, traits, Alloc>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const ElementType& element);

} //Close namespace Internal
} //Close namespace MarshalSupport
#include "CalculateDecomposedSTLContainerSize.inl"
#endif
