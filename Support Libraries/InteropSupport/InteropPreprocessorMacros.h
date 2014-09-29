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

//Define a macro for how to perform an is_base_of type trait check, based on whether C++11
//is supported. We can emulate this check with a C++03 conformant compiler if required.
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <type_traits>
#define INTEROPSUPPORT_ISBASEOF(B, D) std::is_base_of<B, D>::value
#else
#include "IsBaseOfTypeTraitEmulator.h"
#define INTEROPSUPPORT_ISBASEOF(B, D) is_base_of_emulator<B, D>::result
#endif

#endif
