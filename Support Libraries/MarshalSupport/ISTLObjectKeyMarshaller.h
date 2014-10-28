#ifndef __ISTLOBJECTKEYMARSHALLER_H__
#define __ISTLOBJECTKEYMARSHALLER_H__
#include "ISTLObjectKeyMarshallerBase.h"
#include "MarshalPreprocessorMacros.h"
#include "IsLastNestedContainerElement.h"
#include "HasMarshalConstructor.h"
namespace MarshalSupport {
//Forward declare our Marshal::Ret class
namespace Marshal { template <class T> class Ret; }
namespace Internal {

//----------------------------------------------------------------------------------------
template<class KeyType, bool IsLastElement = is_last_nested_container_element<KeyType>::value, bool HasMarshalConstructor = has_marshal_constructor<KeyType>::value>
class ISTLObjectKeyMarshaller :public ISTLObjectKeyMarshallerBase
{
public:
	//Key methods
	virtual void AddKey(const KeyType& key) = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void AddKey(KeyType&& key) = 0;
#endif
	virtual Marshal::Ret<KeyType> RemoveKey() = 0;
};

//----------------------------------------------------------------------------------------
template<class KeyType>
class ISTLObjectKeyMarshaller<KeyType, true, false> :public ISTLObjectKeyMarshallerBase
{
public:
	//Key methods
	virtual void AddKey(const KeyType& key) = 0;
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	virtual void AddKey(KeyType&& key) = 0;
#endif
	virtual KeyType RemoveKey() = 0;
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
