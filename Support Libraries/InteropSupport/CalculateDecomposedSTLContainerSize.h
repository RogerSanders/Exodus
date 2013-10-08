#ifndef __CALCULATEDECOMPOSEDSTLCONTAINERSIZE_H__
#define __CALCULATEDECOMPOSEDSTLCONTAINERSIZE_H__
#include "InteropPreprocessorMacros.h"
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <array>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#endif
namespace InteropSupport {

template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::vector<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::array<ElementType, ArraySize>& element);
#endif
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::list<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::forward_list<ElementType>& element);
#endif
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::deque<ElementType>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::set<ElementType>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multiset<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_set<ElementType>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multiset<ElementType>& element);
#endif
template<class ElementType, class KeyType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::multimap<KeyType, ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, class KeyType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::unordered_multimap<KeyType, ElementType>& element);
#endif
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::stack<ElementType>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::queue<ElementType>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::priority_queue<ElementType>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const std::basic_string<ElementType>& element);
template<class ElementType>
inline void CalculateDecomposedSTLContainerSize(size_t nestingDepth, size_t currentNestingLevel, size_t& elementArrayIndex, size_t& elementSizeArrayIndex, const ElementType& element);

} //Close namespace InteropSupport
#include "CalculateDecomposedSTLContainerSize.inl"
#endif
