#ifndef __ISONLYMOVABLE_H__
#define __ISONLYMOVABLE_H__
#include "MarshalPreprocessorMacros.h"
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <utility>
#include <tuple>
#include <memory>
#include "IsComplete.h"
#endif
namespace MarshalSupport {
namespace Internal {

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
// Define a helper type to determine whether a specific type is movable or not. This helper type will consider all types
// which are not complete, IE, types which have only been declared and not defined, to be copyable. It is impossible to
// determine if the type is in fact only movable until it is complete, and a compilation error will result from
// attempting to use type traits to determine whether a type is movable or copyable unless it is complete. Note that if
// it is necessary to change this assumption for a particular type, the user can still do this, by declaring their own
// specialization of the "is_only_movable" type, returning the required value, IE:
// template<>
// struct is_only_movable<SomeType> { static const bool value = true; };
// This can be done before the type is defined, allowing types which have only been declared and not defined to be used
// in function signatures within headers for example, as long as this specialization is defined when it is declared, or
// prior to its use with the MarshalSupport library.
template<class T, bool isDefined = is_complete<T>::value>
struct is_only_movable_helper { };
template<class T>
struct is_only_movable_helper<T, true> { static const bool value = std::integral_constant<bool, (std::is_move_assignable<T>::value && !std::is_copy_assignable<T>::value) || (std::is_move_constructible<T>::value && !std::is_copy_constructible<T>::value)>::value; };
template <class T>
struct is_only_movable_helper<T, false> { static const bool value = false; };
#endif

// Define our "is_only_movable" type trait
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#ifndef MARSHALSUPPORT_NOVARIADICTEMPLATES
template<class T, class... Args>
struct is_only_movable { static const bool value = std::integral_constant<bool, is_only_movable<T>::value || is_only_movable<Args...>::value>::value; };
template<class T>
struct is_only_movable<T> { static const bool value = std::integral_constant<bool, is_only_movable_helper<T>::value>::value; };
#else
template<class T>
struct is_only_movable { static const bool value = std::integral_constant<bool, is_only_movable_helper<T>::value>::value; };
#endif
template<class T1, class T2>
struct is_only_movable<std::pair<T1, T2>> { static const bool value = std::integral_constant<bool, is_only_movable<T1>::value || is_only_movable<T2>::value>::value; };
#ifndef MARSHALSUPPORT_NOVARIADICTEMPLATES
template<class... Args>
struct is_only_movable<std::tuple<Args...>> { static const bool value = is_only_movable<Args...>::value; };
template<>
struct is_only_movable<std::tuple<>> { static const bool value = false; };
#endif
template<class T, class Deleter>
struct is_only_movable<std::unique_ptr<T, Deleter>> { static const bool value = false; };
#else
template<class T>
struct is_only_movable { static const bool value = false; };
#endif

} // Close namespace Internal
} // Close namespace MarshalSupport
#endif
