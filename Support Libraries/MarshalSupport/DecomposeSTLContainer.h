#ifndef __DECOMPOSESTLCONTAINER_H__
#define __DECOMPOSESTLCONTAINER_H__
#include "MarshalPreprocessorMacros.h"
#include "ISTLObjectKeyMarshallerBase.h"
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
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::vector<ElementType, Alloc>&& element);
#endif
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::vector<bool>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, size_t ArraySize>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::array<ElementType, ArraySize>& element);
template<class ElementType, size_t ArraySize>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::array<ElementType, ArraySize>&& element);
#endif
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::list<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::list<ElementType, Alloc>&& element);
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::forward_list<ElementType, Alloc>& element);
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::forward_list<ElementType, Alloc>&& element);
#endif
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::deque<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::deque<ElementType, Alloc>&& element);
#endif
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::set<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>&& element);
#endif
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::multiset<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>&& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_set<ElementType, Hash, Pred, Alloc>& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>&& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element);
template<class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>&& element);
#endif
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::map<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>&& element);
#endif
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::multimap<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class KeyType, class ElementType, class Compare, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>&& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>&& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element);
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>&& element);
#endif
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::stack<ElementType, Container>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::stack<ElementType, Container>&& element);
#endif
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::queue<ElementType, Container>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Container>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::queue<ElementType, Container>&& element);
#endif
template<class ElementType, class Container, class Compare>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::priority_queue<ElementType, Container, Compare>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class Container, class Compare>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>&& element);
#endif
template<class ElementType, class traits, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const std::basic_string<ElementType, traits, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType, class traits, class Alloc>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>&& element);
#endif
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, const ElementType& element);
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, ElementType& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
template<class ElementType>
inline void DecomposeSTLContainer(void* itemArray, size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, ElementType&& element);
#endif

} //Close namespace Internal
} //Close namespace MarshalSupport
#include "DecomposeSTLContainer.inl"
#endif
