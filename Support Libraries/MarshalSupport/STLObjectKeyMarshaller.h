#ifndef __STLOBJECTKEYMARSHALLER_H__
#define __STLOBJECTKEYMARSHALLER_H__
#include "ISTLObjectKeyMarshaller.h"
#include "MarshalPreprocessorMacros.h"
#include "IsLastNestedContainerElement.h"
#include "HasMarshalConstructor.h"
#include <queue>
namespace MarshalSupport {
namespace Internal {

//----------------------------------------------------------------------------------------
template<class KeyType, bool IsLastElement = is_last_nested_container_element<KeyType>::value, bool HasMarshalConstructor = has_marshal_constructor<KeyType>::value>
class STLObjectKeyMarshaller :public ISTLObjectKeyMarshaller<KeyType>
{
protected:
	//Key methods
	inline virtual void AddKey(const KeyType& key)
	{
		keyList.push(key);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline virtual void AddKey(KeyType&& key)
	{
		keyList.push(std::move(key));
	}
#endif
	inline virtual Marshal::Ret<KeyType> RemoveKey()
	{
		KeyType key(MARSHALSUPPORT_MOVE(keyList.front()));
		keyList.pop();
		return key;
	}

private:
	std::queue<KeyType> keyList;
};

//----------------------------------------------------------------------------------------
template<class KeyType>
class STLObjectKeyMarshaller<KeyType, true, false> :public ISTLObjectKeyMarshaller<KeyType>
{
protected:
	//Key methods
	inline virtual void AddKey(const KeyType& key)
	{
		keyList.push(key);
	}
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	inline virtual void AddKey(KeyType&& key)
	{
		keyList.push(std::move(key));
	}
#endif
	inline virtual KeyType RemoveKey()
	{
		KeyType key(MARSHALSUPPORT_MOVE(keyList.front()));
		keyList.pop();
		return key;
	}

private:
	std::queue<KeyType> keyList;
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
