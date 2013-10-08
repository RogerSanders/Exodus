#ifndef __STLOBJECTKEYMARSHALLER_H__
#define __STLOBJECTKEYMARSHALLER_H__
#include "ISTLObjectKeyMarshaller.h"
#include <queue>
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class KeyType>
class STLObjectKeyMarshaller :public ISTLObjectKeyMarshaller<KeyType>
{
protected:
	//Key methods
	inline virtual void AddKey(KeyType key)
	{
		keyList.push(key);
	}
	inline virtual KeyType RemoveKey()
	{
		KeyType key = keyList.front();
		keyList.pop();
		return key;
	}

private:
	std::queue<KeyType> keyList;
};

//----------------------------------------------------------------------------------------
template<class ElementType>
class STLObjectKeyMarshaller<std::basic_string<ElementType>> :public ISTLObjectKeyMarshaller<std::basic_string<ElementType>>
{
protected:
	//Key methods
	inline virtual void AddKey(const ElementType* keyData, size_t keyDataLength)
	{
		std::basic_string<ElementType> key(keyData, keyDataLength);
		keyList.push(key);
	}
	inline virtual void RetrieveNextKey(const ElementType*& keyData, size_t& keyDataLength) const
	{
		const std::basic_string<ElementType>& key = keyList.front();
		keyData = key.data();
		keyDataLength = key.size();
	}
	inline virtual void PopNextKey()
	{
		keyList.pop();
	}

private:
	std::queue<std::basic_string<ElementType>> keyList;
};

} //Close namespace InteropSupport
#endif
