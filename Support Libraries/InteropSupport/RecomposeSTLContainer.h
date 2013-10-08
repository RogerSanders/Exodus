#ifndef __RECOMPOSESTLCONTAINER_H__
#define __RECOMPOSESTLCONTAINER_H__
#include "InteropPreprocessorMacros.h"
#include "ISTLObjectKeyMarshallerBase.h"
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
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::vector<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::array<ElementType, ArraySize>& element);
#endif
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::list<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::forward_list<ElementType>& element);
#endif
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::deque<ElementType>& element);
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::set<ElementType>& element);
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multiset<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_set<ElementType>& element);
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multiset<ElementType>& element);
#endif
template<class ElementType, class KeyType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multimap<KeyType, ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, class KeyType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType>& element);
#endif
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::stack<ElementType>& element);
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::queue<ElementType>& element);
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::priority_queue<ElementType>& element);
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::basic_string<ElementType>& element);
template<class ElementType>
inline void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, ElementType& element);

} //Close namespace InteropSupport
#include "RecomposeSTLContainer.inl"
#endif
