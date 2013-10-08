#ifndef __DECOMPOSESTLCONTAINER_H__
#define __DECOMPOSESTLCONTAINER_H__
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
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::vector<ElementType>& element);
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::vector<bool>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::array<ElementType, ArraySize>& element);
#endif
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::list<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::forward_list<ElementType>& element);
#endif
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::deque<ElementType>& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::set<ElementType>& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::multiset<ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_set<ElementType>& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_multiset<ElementType>& element);
#endif
template<class ElementType, class KeyType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::multimap<KeyType, ElementType>& element);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
template<class ElementType, class KeyType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType>& element);
template<class ElementType, class KeyType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType>& element);
#endif
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::stack<ElementType>& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::queue<ElementType>& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::priority_queue<ElementType>& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::basic_string<ElementType>& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const ElementType& element);

} //Close namespace InteropSupport
#include "DecomposeSTLContainer.inl"
#endif
