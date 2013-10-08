#include "STLObjectKeyMarshaller.h"
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::vector<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::array<ElementType, ArraySize>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::list<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::forward_list<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::deque<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::set<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multiset<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_set<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multiset<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::map<KeyType, ElementType>* elementPointer)
{
	keyMarshallerArray[keyMarshallerArrayIndex] = new STLObjectKeyMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::multimap<KeyType, ElementType>* elementPointer)
{
	keyMarshallerArray[keyMarshallerArrayIndex] = new STLObjectKeyMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_map<KeyType, ElementType>* elementPointer)
{
	keyMarshallerArray[keyMarshallerArrayIndex] = new STLObjectKeyMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::unordered_multimap<KeyType, ElementType>* elementPointer)
{
	keyMarshallerArray[keyMarshallerArrayIndex] = new STLObjectKeyMarshaller<KeyType>();
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::stack<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::queue<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const std::priority_queue<ElementType>* elementPointer)
{
	CreateSTLContainerKeyMarshallers(keyMarshallerArray, keyMarshallerArrayIndex+1, (const ElementType*)elementPointer);
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void CreateSTLContainerKeyMarshallers(ISTLObjectKeyMarshallerBase* keyMarshallerArray[], size_t keyMarshallerArrayIndex, const ElementType* elementPointer)
{ }

} //Close namespace InteropSupport
