#ifndef __MARSHALOBJECT_H__
#define __MARSHALOBJECT_H__
#include "MarshalObjectT.h"
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#endif
#include "HasMarshalConstructor.h"
#include "GetNextNestedContainerElementType.h"
namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class ElementType, bool HasMashalConstructor = has_marshal_constructor<ElementType>::value>
class MarshalObjectHelperInternal
{ };

//----------------------------------------------------------------------------------------
template<class ElementType>
class MarshalObjectHelperInternal<ElementType, false>
{
public:
	inline static void MarshalObjectToExistingObject(const ElementType& sourceObject, ElementType& targetObject)
	{
		targetObject = sourceObject;
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToExistingObject(ElementType&& sourceObject, ElementType& targetObject)
	{
		targetObject = std::move(sourceObject);
	}
#endif
	inline static ElementType MarshalObjectToNewObject(const ElementType& sourceObject)
	{
		return ElementType(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static ElementType MarshalObjectToNewObject(ElementType&& sourceObject)
	{
		return ElementType(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType>
class MarshalObjectHelperInternal<ElementType, true>
{
public:
	inline static void MarshalObjectToExistingObject(const ElementType& sourceObject, ElementType& targetObject)
	{
		targetObject = ElementType(marshal_object_t(), sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToExistingObject(ElementType&& sourceObject, ElementType& targetObject)
	{
		targetObject = ElementType(marshal_object_t(), std::move(sourceObject));
	}
#endif
	inline static ElementType MarshalObjectToNewObject(const ElementType& sourceObject)
	{
		return ElementType(marshal_object_t(), sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static ElementType MarshalObjectToNewObject(ElementType&& sourceObject)
	{
		return ElementType(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, bool ContainedElementHasMashalConstructor = has_marshal_constructor<typename get_next_nested_container_element_type<ElementType>::type>::value>
class MarshalObjectHelper :public MarshalObjectHelperInternal<ElementType>
{ };

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::vector<ElementType, Alloc>, false> :public MarshalObjectHelperInternal<std::vector<ElementType, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::vector<ElementType, Alloc>& targetObject)
	{
		targetObject.push_back(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::vector<ElementType, Alloc>& targetObject)
	{
		targetObject.push_back(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::vector<ElementType, Alloc>, true> :public MarshalObjectHelperInternal<std::vector<ElementType, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::vector<ElementType, Alloc>& targetObject)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		targetObject.emplace_back(marshal_object_t(), sourceObject);
#else
		targetObject.push_back(ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::vector<ElementType, Alloc>& targetObject)
	{
		targetObject.emplace_back(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::list<ElementType, Alloc>, false> :public MarshalObjectHelperInternal<std::list<ElementType, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::list<ElementType, Alloc>& targetObject)
	{
		targetObject.push_back(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::list<ElementType, Alloc>& targetObject)
	{
		targetObject.push_back(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::list<ElementType, Alloc>, true> :public MarshalObjectHelperInternal<std::list<ElementType, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::list<ElementType, Alloc>& targetObject)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		targetObject.emplace_back(marshal_object_t(), sourceObject);
#else
		targetObject.push_back(ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::list<ElementType, Alloc>& targetObject)
	{
		targetObject.emplace_back(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::forward_list<ElementType, Alloc>, false> :public MarshalObjectHelperInternal<std::forward_list<ElementType, Alloc>>
{
public:
	inline static typename std::forward_list<ElementType, Alloc>::iterator MarshalObjectToContainer(const ElementType& sourceObject, std::forward_list<ElementType, Alloc>& targetObject, const typename std::forward_list<ElementType, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert_after(insertPosition, sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::forward_list<ElementType, Alloc>::iterator MarshalObjectToContainer(ElementType&& sourceObject, std::forward_list<ElementType, Alloc>& targetObject, const typename std::forward_list<ElementType, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert_after(insertPosition, std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::forward_list<ElementType, Alloc>, true> :public MarshalObjectHelperInternal<std::forward_list<ElementType, Alloc>>
{
public:
	inline static typename std::forward_list<ElementType, Alloc>::iterator MarshalObjectToContainer(const ElementType& sourceObject, std::forward_list<ElementType, Alloc>& targetObject, const typename std::forward_list<ElementType, Alloc>::iterator& insertPosition)
	{
		return targetObject.emplace_after(insertPosition, marshal_object_t(), sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::forward_list<ElementType, Alloc>::iterator MarshalObjectToContainer(ElementType&& sourceObject, std::forward_list<ElementType, Alloc>& targetObject, const typename std::forward_list<ElementType, Alloc>::iterator& insertPosition)
	{
		return targetObject.emplace_after(insertPosition, marshal_object_t(), std::move(sourceObject));
	}
#endif
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::deque<ElementType, Alloc>, false> :public MarshalObjectHelperInternal<std::deque<ElementType, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::deque<ElementType, Alloc>& targetObject)
	{
		targetObject.push_back(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::deque<ElementType, Alloc>& targetObject)
	{
		targetObject.push_back(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Alloc>
class MarshalObjectHelper<std::deque<ElementType, Alloc>, true> :public MarshalObjectHelperInternal<std::deque<ElementType, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::deque<ElementType, Alloc>& targetObject)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		targetObject.emplace_back(marshal_object_t(), sourceObject);
#else
		targetObject.push_back(ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::deque<ElementType, Alloc>& targetObject)
	{
		targetObject.emplace_back(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::set<ElementType, Compare, Alloc>, false> :public MarshalObjectHelperInternal<std::set<ElementType, Compare, Alloc>>
{
public:
	inline static typename std::set<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const ElementType& sourceObject, std::set<ElementType, Compare, Alloc>& targetObject, const typename std::set<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::set<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(ElementType&& sourceObject, std::set<ElementType, Compare, Alloc>& targetObject, const typename std::set<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::set<ElementType, Compare, Alloc>, true> :public MarshalObjectHelperInternal<std::set<ElementType, Compare, Alloc>>
{
public:
	inline static typename std::set<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const ElementType& sourceObject, std::set<ElementType, Compare, Alloc>& targetObject, const typename std::set<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		return targetObject.emplace_hint(insertPosition, marshal_object_t(), sourceObject);
#else
		return targetObject.insert(insertPosition, ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::set<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(ElementType&& sourceObject, std::set<ElementType, Compare, Alloc>& targetObject, const typename std::set<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.emplace_hint(insertPosition, marshal_object_t(), std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::multiset<ElementType, Compare, Alloc>, false> :public MarshalObjectHelperInternal<std::multiset<ElementType, Compare, Alloc>>
{
public:
	inline static typename std::multiset<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const ElementType& sourceObject, std::multiset<ElementType, Compare, Alloc>& targetObject, const typename std::multiset<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::multiset<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(ElementType&& sourceObject, std::multiset<ElementType, Compare, Alloc>& targetObject, const typename std::multiset<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::multiset<ElementType, Compare, Alloc>, true> :public MarshalObjectHelperInternal<std::multiset<ElementType, Compare, Alloc>>
{
public:
	inline static typename std::multiset<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const ElementType& sourceObject, std::multiset<ElementType, Compare, Alloc>& targetObject, const typename std::multiset<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		return targetObject.emplace_hint(insertPosition, marshal_object_t(), sourceObject);
#else
		return targetObject.insert(insertPosition, ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::multiset<ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(ElementType&& sourceObject, std::multiset<ElementType, Compare, Alloc>& targetObject, const typename std::multiset<ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.emplace_hint(insertPosition, marshal_object_t(), std::move(sourceObject));
	}
#endif
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_set<ElementType, Hash, Pred, Alloc>, false> :public MarshalObjectHelperInternal<std::unordered_set<ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::unordered_set<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(sourceObject);
	}
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::unordered_set<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(std::move(sourceObject));
	}
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_set<ElementType, Hash, Pred, Alloc>, true> :public MarshalObjectHelperInternal<std::unordered_set<ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::unordered_set<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(marshal_object_t(), sourceObject);
	}
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::unordered_set<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(marshal_object_t(), std::move(sourceObject));
	}
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_multiset<ElementType, Hash, Pred, Alloc>, false> :public MarshalObjectHelperInternal<std::unordered_multiset<ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(sourceObject);
	}
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(std::move(sourceObject));
	}
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_multiset<ElementType, Hash, Pred, Alloc>, true> :public MarshalObjectHelperInternal<std::unordered_multiset<ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(marshal_object_t(), sourceObject);
	}
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::unordered_multiset<ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(marshal_object_t(), std::move(sourceObject));
	}
};
#endif

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::map<KeyType, ElementType, Compare, Alloc>, false> :public MarshalObjectHelperInternal<std::map<KeyType, ElementType, Compare, Alloc>>
{
public:
	inline static typename std::map<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::map<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::map<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, std::make_pair(keyValue, sourceObject));
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::map<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::map<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::map<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, std::make_pair(keyValue, std::move(sourceObject)));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::map<KeyType, ElementType, Compare, Alloc>, true> :public MarshalObjectHelperInternal<std::map<KeyType, ElementType, Compare, Alloc>>
{
public:
	inline static typename std::map<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::map<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::map<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		return targetObject.emplace_hint(insertPosition, keyValue, ElementType(marshal_object_t(), sourceObject));
#else
		return targetObject.insert(insertPosition, std::make_pair(keyValue, ElementType(marshal_object_t(), sourceObject)));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::map<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::map<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::map<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.emplace_hint(insertPosition, keyValue, ElementType(marshal_object_t(), std::move(sourceObject)));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::multimap<KeyType, ElementType, Compare, Alloc>, false> :public MarshalObjectHelperInternal<std::multimap<KeyType, ElementType, Compare, Alloc>>
{
public:
	inline static typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::multimap<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, std::make_pair(keyValue, sourceObject));
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::multimap<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.insert(insertPosition, std::make_pair(keyValue, std::move(sourceObject)));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Compare, class Alloc>
class MarshalObjectHelper<std::multimap<KeyType, ElementType, Compare, Alloc>, true> :public MarshalObjectHelperInternal<std::multimap<KeyType, ElementType, Compare, Alloc>>
{
public:
	inline static typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::multimap<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		return targetObject.emplace_hint(insertPosition, keyValue, marshal_object_t(), sourceObject);
#else
		return targetObject.insert(insertPosition, std::make_pair(keyValue, ElementType(marshal_object_t(), sourceObject)));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::multimap<KeyType, ElementType, Compare, Alloc>& targetObject, const typename std::multimap<KeyType, ElementType, Compare, Alloc>::iterator& insertPosition)
	{
		return targetObject.emplace_hint(insertPosition, keyValue, ElementType(marshal_object_t(), std::move(sourceObject)));
	}
#endif
};

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>, false> :public MarshalObjectHelperInternal<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(std::make_pair(keyValue, sourceObject));
	}
	inline static void MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(std::make_pair(keyValue, std::move(sourceObject)));
	}
};

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>, true> :public MarshalObjectHelperInternal<std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(keyValue, marshal_object_t(), sourceObject);
	}
	inline static void MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::unordered_map<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(keyValue, ElementType(marshal_object_t(), std::move(sourceObject)));
	}
};

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>, false> :public MarshalObjectHelperInternal<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(std::make_pair(keyValue, sourceObject));
	}
	inline static void MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.insert(std::make_pair(keyValue, std::move(sourceObject)));
	}
};

//----------------------------------------------------------------------------------------
template<class KeyType, class ElementType, class Hash, class Pred, class Alloc>
class MarshalObjectHelper<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>, true> :public MarshalObjectHelperInternal<std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const KeyType& keyValue, const ElementType& sourceObject, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(keyValue, marshal_object_t(), sourceObject);
	}
	inline static void MarshalObjectToContainer(const KeyType& keyValue, ElementType&& sourceObject, std::unordered_multimap<KeyType, ElementType, Hash, Pred, Alloc>& targetObject)
	{
		targetObject.emplace(keyValue, ElementType(marshal_object_t(), std::move(sourceObject)));
	}
};
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
class MarshalObjectHelper<std::stack<ElementType, Container>, false> :public MarshalObjectHelperInternal<std::stack<ElementType, Container>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::stack<ElementType, Container>& targetObject)
	{
		targetObject.push(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::stack<ElementType, Container>& targetObject)
	{
		targetObject.push(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
class MarshalObjectHelper<std::stack<ElementType, Container>, true> :public MarshalObjectHelperInternal<std::stack<ElementType, Container>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::stack<ElementType, Container>& targetObject)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		targetObject.emplace(marshal_object_t(), sourceObject);
#else
		targetObject.push(ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::stack<ElementType, Container>& targetObject)
	{
		targetObject.emplace(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
class MarshalObjectHelper<std::queue<ElementType, Container>, false> :public MarshalObjectHelperInternal<std::queue<ElementType, Container>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::queue<ElementType, Container>& targetObject)
	{
		targetObject.push(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::queue<ElementType, Container>& targetObject)
	{
		targetObject.push(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Container>
class MarshalObjectHelper<std::queue<ElementType, Container>, true> :public MarshalObjectHelperInternal<std::queue<ElementType, Container>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::queue<ElementType, Container>& targetObject)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		targetObject.emplace(marshal_object_t(), sourceObject);
#else
		targetObject.push(ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::queue<ElementType, Container>& targetObject)
	{
		targetObject.emplace(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
class MarshalObjectHelper<std::priority_queue<ElementType, Container, Compare>, false> :public MarshalObjectHelperInternal<std::priority_queue<ElementType, Container, Compare>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::priority_queue<ElementType, Container, Compare>& targetObject)
	{
		targetObject.push(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::priority_queue<ElementType, Container, Compare>& targetObject)
	{
		targetObject.push(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class Container, class Compare>
class MarshalObjectHelper<std::priority_queue<ElementType, Container, Compare>, true> :public MarshalObjectHelperInternal<std::priority_queue<ElementType, Container, Compare>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::priority_queue<ElementType, Container, Compare>& targetObject)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		targetObject.emplace(marshal_object_t(), sourceObject);
#else
		targetObject.push(ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::priority_queue<ElementType, Container, Compare>& targetObject)
	{
		targetObject.emplace(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>, false> :public MarshalObjectHelperInternal<std::basic_string<ElementType, traits, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::basic_string<ElementType, traits, Alloc>& targetObject)
	{
		targetObject.push_back(sourceObject);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::basic_string<ElementType, traits, Alloc>& targetObject)
	{
		targetObject.push_back(std::move(sourceObject));
	}
#endif
};

//----------------------------------------------------------------------------------------
template<class ElementType, class traits, class Alloc>
class MarshalObjectHelper<std::basic_string<ElementType, traits, Alloc>, true> :public MarshalObjectHelperInternal<std::basic_string<ElementType, traits, Alloc>>
{
public:
	inline static void MarshalObjectToContainer(const ElementType& sourceObject, std::basic_string<ElementType, traits, Alloc>& targetObject)
	{
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
		targetObject.emplace_back(marshal_object_t(), sourceObject);
#else
		targetObject.push_back(ElementType(marshal_object_t(), sourceObject));
#endif
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline static void MarshalObjectToContainer(ElementType&& sourceObject, std::basic_string<ElementType, traits, Alloc>& targetObject)
	{
		targetObject.emplace_back(marshal_object_t(), std::move(sourceObject));
	}
#endif
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
