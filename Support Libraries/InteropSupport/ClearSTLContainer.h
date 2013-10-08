#ifndef __CLEARSTLCONTAINER_H__
#define __CLEARSTLCONTAINER_H__
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
inline void ClearSTLContainer(std::vector<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void ClearSTLContainer(std::array<ElementType, ArraySize>& element);
#endif
template<class ElementType>
inline void ClearSTLContainer(std::list<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void ClearSTLContainer(std::forward_list<ElementType>& element);
#endif
template<class ElementType>
inline void ClearSTLContainer(std::deque<ElementType>& element);
template<class ElementType>
inline void ClearSTLContainer(std::set<ElementType>& element);
template<class ElementType>
inline void ClearSTLContainer(std::multiset<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void ClearSTLContainer(std::unordered_set<ElementType>& element);
template<class ElementType>
inline void ClearSTLContainer(std::unordered_multiset<ElementType>& element);
#endif
template<class ElementType, class KeyType>
inline void ClearSTLContainer(std::map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void ClearSTLContainer(std::multimap<KeyType, ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, class KeyType>
inline void ClearSTLContainer(std::unordered_map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void ClearSTLContainer(std::unordered_multimap<KeyType, ElementType>& element);
#endif
template<class ElementType>
inline void ClearSTLContainer(std::stack<ElementType>& element);
template<class ElementType>
inline void ClearSTLContainer(std::queue<ElementType>& element);
template<class ElementType>
inline void ClearSTLContainer(std::priority_queue<ElementType>& element);
template<class ElementType>
inline void ClearSTLContainer(std::basic_string<ElementType>& element);
template<class ElementType>
inline void ClearSTLContainer(ElementType& element);

} //Close namespace InteropSupport
#include "ClearSTLContainer.inl"
#endif
