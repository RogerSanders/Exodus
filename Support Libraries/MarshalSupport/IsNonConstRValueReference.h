#ifndef __ISNONCONSTRVALUEREFERENCE_H__
#define __ISNONCONSTRVALUEREFERENCE_H__
#include "MarshalPreprocessorMacros.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#endif
namespace MarshalSupport {
namespace Internal {

template<class T>
struct is_non_const_rvalue_reference
{
public:
	//Result
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
	static const bool value = std::is_rvalue_reference<T>::value && !std::is_const<T>::value;
#else
	static const bool value = false;
#endif
};

} //Close namespace Internal
} //Close namespace MarshalSupport
#endif
