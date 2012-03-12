/*
Things to do:
-Implement our own custom assert functions
-Add the ability to stream in any custom error message, with parameters, that can be
constructed using stream I/O.
-Go back through all our existing code and implement assertions system-wide to report
errors
-Our custom assertion failure dialog should have an option to submit the minidump file
for that failure over the internet there and then
-Make sure we can ignore a custom assertion failure and continue execution. Give the user
the choice to "Terminate", "Ignore", or "Debug".
*/

#ifndef __ASSERTION_H__
#define __ASSERTION_H__

//Assert functions
inline void Assert(bool expression, unsigned int line, const wchar_t* file, const wchar_t* message);

//Assert macros
#define ASSERTFAILUREEXCEPTIONCODE 0xDEADC0DE
#define MAKESTRINGWIDE(string) L ## string
#ifdef  NDEBUG
#define DebugAssert(expr)
#define ReleaseAssert(expr)	Assert(expr, __LINE__, _CRT_WIDE(__FILE__), _CRT_WIDE(#expr))
#else
#define DebugAssert(expr)	Assert(expr, __LINE__, _CRT_WIDE(__FILE__), _CRT_WIDE(#expr))
#define ReleaseAssert(expr)	Assert(expr, __LINE__, _CRT_WIDE(__FILE__), _CRT_WIDE(#expr))
#endif

#include "Assertion.inl"
#endif
