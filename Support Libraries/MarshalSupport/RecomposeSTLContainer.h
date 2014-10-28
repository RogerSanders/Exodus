#ifndef __RECOMPOSESTLCONTAINER_H__
#define __RECOMPOSESTLCONTAINER_H__
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

//----------------------------------------------------------------------------------------
template<bool IsLastElement>
class RecomposeSTLContainerHelper
{
public:
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, size_t ArraySize>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::array<ElementType, ArraySize>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::list<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::deque<ElementType, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element);
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element);
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element);
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::stack<ElementType, Container>& element);
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::queue<ElementType, Container>& element);
	template<class ElementType, class Container, class Compare>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element);
	template<class ElementType, class traits, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element);
};

//----------------------------------------------------------------------------------------
template<>
class RecomposeSTLContainerHelper<true>
{
public:
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, size_t ArraySize>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::array<ElementType, ArraySize>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::list<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::deque<ElementType, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element);
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element);
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element);
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::stack<ElementType, Container>& element);
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::queue<ElementType, Container>& element);
	template<class ElementType, class Container, class Compare>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element);
	template<class ElementType, class traits, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element);
	template<class ElementType>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, ElementType& element);
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#include "RecomposeSTLContainer.inl"
#endif
