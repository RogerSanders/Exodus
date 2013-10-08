#ifndef __INTEROPPREPROCESSORMACROS_H__
#define __INTEROPPREPROCESSORMACROS_H__

//Detect the level of C++11 support provided by the current compiler
#ifndef INTEROPSUPPORT_CPP11SUPPORTED
#if __cplusplus >= 201103L
#define INTEROPSUPPORT_CPP11SUPPORTED
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define INTEROPSUPPORT_CPP11SUPPORTED
#define INTEROPSUPPORT_NOCONSTEXPR
#endif
#endif

//Define a macro for the constexpr keyword, based on whether C++11 is supported by the
//current compiler.
#ifndef INTEROPSUPPORT_CONSTEXPR
#if defined(INTEROPSUPPORT_CPP11SUPPORTED) && !defined(INTEROPSUPPORT_NOCONSTEXPR)
#define INTEROPSUPPORT_CONSTEXPR constexpr
#else
#define INTEROPSUPPORT_CONSTEXPR
#endif
#endif

#endif
