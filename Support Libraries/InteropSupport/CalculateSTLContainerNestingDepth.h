#ifndef __CALCULATESTLCONTAINERNESTINGDEPTH_H__
#define __CALCULATESTLCONTAINERNESTINGDEPTH_H__
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
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::vector<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::array<ElementType, ArraySize>* elementPointer);
#endif
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::list<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::forward_list<ElementType>* elementPointer);
#endif
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::deque<ElementType>* elementPointer);
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::set<ElementType>* elementPointer);
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::multiset<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_set<ElementType>* elementPointer);
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_multiset<ElementType>* elementPointer);
#endif
template<class ElementType, class KeyType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::map<KeyType, ElementType>* elementPointer);
template<class ElementType, class KeyType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::multimap<KeyType, ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, class KeyType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_map<KeyType, ElementType>* elementPointer);
template<class ElementType, class KeyType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::unordered_multimap<KeyType, ElementType>* elementPointer);
#endif
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::stack<ElementType>* elementPointer);
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::queue<ElementType>* elementPointer);
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::priority_queue<ElementType>* elementPointer);
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const std::basic_string<ElementType>* elementPointer);
template<class ElementType>
inline INTEROPSUPPORT_CONSTEXPR size_t CalculateSTLContainerNestingDepth(size_t currentNestingDepth, const ElementType* elementPointer);

} //Close namespace InteropSupport
#include "CalculateSTLContainerNestingDepth.inl"
#endif
