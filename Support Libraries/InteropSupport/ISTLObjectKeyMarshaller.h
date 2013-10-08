#ifndef __ISTLOBJECTKEYMARSHALLER_H__
#define __ISTLOBJECTKEYMARSHALLER_H__
#include "ISTLObjectKeyMarshallerBase.h"
#include <string>
namespace InteropSupport {

//----------------------------------------------------------------------------------------
template<class KeyType>
class ISTLObjectKeyMarshaller :public ISTLObjectKeyMarshallerBase
{
public:
	//Key methods
	virtual void AddKey(KeyType key) = 0;
	virtual KeyType RemoveKey() = 0;
};

//----------------------------------------------------------------------------------------
template<class ElementType>
class ISTLObjectKeyMarshaller<std::basic_string<ElementType>> :public ISTLObjectKeyMarshallerBase
{
public:
	//Key methods
	inline void AddKey(const std::basic_string<ElementType>& key)
	{
		AddKey(key.data(), key.size());
	}
	inline std::basic_string<ElementType> RemoveKey()
	{
		const ElementType* keyData;
		size_t keyDataLength;
		RetrieveNextKey(keyData, keyDataLength);
		std::basic_string<ElementType> key(keyData, keyDataLength);
		PopNextKey();
		return key;
	}

protected:
	//Key methods
	virtual void AddKey(const ElementType* keyData, size_t keyDataLength) = 0;
	virtual void RetrieveNextKey(const ElementType*& keyData, size_t& keyDataLength) const = 0;
	virtual void PopNextKey() = 0;
};

} //Close namespace InteropSupport
#endif
