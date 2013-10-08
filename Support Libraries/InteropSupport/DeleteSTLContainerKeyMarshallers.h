#ifndef __DELETESTLCONTAINERKEYMARSHALLERS_H__
#define __DELETESTLCONTAINERKEYMARSHALLERS_H__
#include "InteropPreprocessorMacros.h"
#include "ISTLObjectKeyMarshallerBase.h"
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <array>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#endif
namespace InteropSupport {

template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::vector<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::array<ElementType, ArraySize>* elementPointer);
#endif
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::list<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::forward_list<ElementType>* elementPointer);
#endif
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::deque<ElementType>* elementPointer);
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::set<ElementType>* elementPointer);
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multiset<ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_set<ElementType>* elementPointer);
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multiset<ElementType>* elementPointer);
#endif
template<class ElementType, class KeyType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::map<KeyType, ElementType>* elementPointer);
template<class ElementType, class KeyType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multimap<KeyType, ElementType>* elementPointer);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, class KeyType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType>* elementPointer);
template<class ElementType, class KeyType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType>* elementPointer);
#endif
template<class ElementType>
inline void DeleteSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const ElementType* elementPointer);

} //Close namespace InteropSupport
#include "DeleteSTLContainerKeyMarshallers.inl"
#endif
