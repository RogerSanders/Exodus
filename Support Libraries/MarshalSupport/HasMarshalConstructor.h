#ifndef __HASMARSHALCONSTRUCTOR_H__
#define __HASMARSHALCONSTRUCTOR_H__
#include "MarshalPreprocessorMacros.h"
#include "MarshalObjectT.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#else
#include "IsBaseOfEmulator.h"
#include "IMarshallingObject.h"
#endif
namespace MarshalSupport {
namespace Internal {

template<class T>
struct has_marshal_constructor
{
public:
	//Result
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	static const bool value = std::is_constructible<T, marshal_object_t, const T&>::value;
#else
	static const bool value = is_base_of_emulator<IMarshallingObject, T>::value;
#endif
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
