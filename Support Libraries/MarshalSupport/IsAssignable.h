#ifndef __ISASSIGNABLE_H__
#define __ISASSIGNABLE_H__
#include "MarshalPreprocessorMacros.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <utility>
#include <tuple>
#include <memory>
#endif
namespace MarshalSupport {
namespace Internal {

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#ifndef MARSHALSUPPORT_NOVARIADICTEMPLATES
template<class T, class... Args>
struct is_assignable { static const bool value = std::integral_constant<bool, is_assignable<T>::value || is_assignable<Args...>::value>::value; };
template<class T>
struct is_assignable<T> { static const bool value = std::integral_constant<bool, std::is_move_assignable<T>::value || std::is_copy_assignable<T>::value>::value; };
#else
template<class T>
struct is_assignable { static const bool value = std::integral_constant<bool, std::is_move_assignable<T>::value || std::is_copy_assignable<T>::value>::value; };
#endif
template<class T1, class T2>
struct is_assignable<std::pair<T1, T2>> { static const bool value = std::integral_constant<bool, is_assignable<T1>::value || is_assignable<T2>::value>::value; };
#ifndef MARSHALSUPPORT_NOVARIADICTEMPLATES
template<class... Args>
struct is_assignable<std::tuple<Args...>> { static const bool value = is_assignable<Args...>::value; };
template<>
struct is_assignable<std::tuple<>> { static const bool value = true; };
#endif
template<class T, class Deleter>
struct is_assignable<std::unique_ptr<T, Deleter>> { static const bool value = true; };
#else
template<class T>
struct is_assignable { static const bool value = true; };
#endif

} // Close namespace Internal
} // Close namespace MarshalSupport
#endif
