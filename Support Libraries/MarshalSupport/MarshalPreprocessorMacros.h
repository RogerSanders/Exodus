#ifndef __MARSHALPREPROCESSORMACROS_H__
#define __MARSHALPREPROCESSORMACROS_H__

//Detect the level of C++11 support provided by the current compiler
#ifndef MARSHALSUPPORT_CPP11SUPPORTED
#if __cplusplus >= 201103L
#define MARSHALSUPPORT_CPP11SUPPORTED
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define MARSHALSUPPORT_CPP11SUPPORTED
#define MARSHALSUPPORT_NOCONSTEXPR
#endif
#endif

//Include the utility namespace for type traits if C++11 is supported
#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#include <utility>
#endif

//Define macros for various C++11 features based on whether C++11 is supported or not
#if defined(MARSHALSUPPORT_CPP11SUPPORTED) && !defined(MARSHALSUPPORT_NOCONSTEXPR)
#define MARSHALSUPPORT_CONSTEXPR constexpr
#else
#define MARSHALSUPPORT_CONSTEXPR
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#define MARSHALSUPPORT_DELETEMETHOD = delete
#else
#define MARSHALSUPPORT_DELETEMETHOD
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#define MARSHALSUPPORT_UNIVERSALREFERENCE(T) T&&
#else
#define MARSHALSUPPORT_UNIVERSALREFERENCE(T) const T&
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#define MARSHALSUPPORT_FORWARD(T, O) std::forward<T>(O)
#else
#define MARSHALSUPPORT_FORWARD(T, O) O
#endif

#ifdef MARSHALSUPPORT_CPP11SUPPORTED
#define MARSHALSUPPORT_MOVE(O) std::move(O)
#else
#define MARSHALSUPPORT_MOVE(O) O
#endif

#endif
