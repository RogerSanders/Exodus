#ifndef __CREATESTLCONTAINERITEMARRAY_H__
#define __CREATESTLCONTAINERITEMARRAY_H__
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
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::vector<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::array<ElementType, ArraySize>* elementPointer);
#endif
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::list<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::forward_list<ElementType>* elementPointer);
#endif
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::deque<ElementType>* elementPointer);
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::set<ElementType>* elementPointer);
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::multiset<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_set<ElementType>* elementPointer);
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_multiset<ElementType>* elementPointer);
#endif
template<class ElementType, class KeyType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::map<KeyType, ElementType>* elementPointer);
template<class ElementType, class KeyType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::multimap<KeyType, ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, class KeyType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_map<KeyType, ElementType>* elementPointer);
template<class ElementType, class KeyType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::unordered_multimap<KeyType, ElementType>* elementPointer);
#endif
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::stack<ElementType>* elementPointer);
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::queue<ElementType>* elementPointer);
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::priority_queue<ElementType>* elementPointer);
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const std::basic_string<ElementType>* elementPointer);
template<class ElementType>
inline void* CreateSTLContainerItemArray(size_t itemArraySize, const ElementType* elementPointer);

} //Close namespace InteropSupport
#include "CreateSTLContainerItemArray.inl"
#endif
