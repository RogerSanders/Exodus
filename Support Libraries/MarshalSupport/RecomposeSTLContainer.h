#ifndef __RECOMPOSESTLCONTAINER_H__
#define __RECOMPOSESTLCONTAINER_H__
#include "MarshalPreprocessorMacros.h"
#include "INestedMarshallerBase.h"
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include <utility>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <array>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#endif
namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------------------------------------
template<class ElementType>
inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element);

//----------------------------------------------------------------------------------------------------------------------
template<bool IsLastElement, bool IsOnlyMovable>
class RecomposeSTLContainerHelper
{
public:
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, size_t ArraySize>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::list<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::deque<ElementType, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element);
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element);
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element);
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::stack<ElementType, Container>& element);
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::queue<ElementType, Container>& element);
	template<class ElementType, class Container, class Compare>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element);
	template<class ElementType, class traits, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element);
};

//----------------------------------------------------------------------------------------------------------------------
template<bool IsOnlyMovable>
class RecomposeSTLContainerHelper<true, IsOnlyMovable>
{
public:
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, size_t ArraySize>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::list<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::forward_list<ElementType, Alloc>& element);
#endif
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::deque<ElementType, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::set<ElementType, Compare, Alloc>& element);
	template<class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multiset<ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_set<ElementType, Hash, Pred, Alloc>& element);
	template<class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::map<KeyType, ElementType, Compare, Alloc>& element);
	template<class KeyType, class ElementType, class Compare, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::multimap<KeyType, ElementType, Compare, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& element);
	template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& element);
#endif
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::stack<ElementType, Container>& element);
	template<class ElementType, class Container>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::queue<ElementType, Container>& element);
	template<class ElementType, class Container, class Compare>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::priority_queue<ElementType, Container, Compare>& element);
	template<class ElementType, class traits, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::basic_string<ElementType, traits, Alloc>& element);
	template<class T1, class T2>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::pair<T1, T2>& element);
#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOVARIADICTEMPLATES)
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<>& element);
	template<class... Args>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::tuple<Args...>& element);
#endif
	template<class ElementType>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element);
};

//----------------------------------------------------------------------------------------------------------------------
template<>
class RecomposeSTLContainerHelper<true, true>
{
public:
	template<class ElementType, class Alloc>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::vector<ElementType, Alloc>& element);
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	template<class ElementType, size_t ArraySize>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, std::array<ElementType, ArraySize>& element);
#endif
	template<class ElementType>
	static inline void RecomposeSTLContainer(size_t elementByteSize, void* itemArray, const size_t elementSizeArray[], INestedMarshallerBase* const nestedMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t nestedMarshallerArrayIndex, ElementType& element);
};

} // Close namespace Internal
} // Close namespace MarshalSupport
#include "RecomposeSTLContainer.inl"
#endif
