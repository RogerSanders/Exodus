#ifndef __INTERLOCKEDTYPES_H__
#define __INTERLOCKEDTYPES_H__
#include "WindowFunctions/WindowFunctions.pkg"

//In VS2005, these intrinsics are defined within the WinNT.h file as well as intrin.h, so
//including both results in a compilation error. We work around that issue here by forcing
//the second set of definitions in intrin.h to use a different name.
#if _MSC_VER == 1400
#define _interlockedbittestandset _interlockedbittestandset_NAME_CHANGED_TO_AVOID_MSVS2005_ERROR
#define _interlockedbittestandreset _interlockedbittestandreset_NAME_CHANGED_TO_AVOID_MSVS2005_ERROR
#define _interlockedbittestandset64 _interlockedbittestandset64_NAME_CHANGED_TO_AVOID_MSVS2005_ERROR
#define _interlockedbittestandreset64 _interlockedbittestandreset64_NAME_CHANGED_TO_AVOID_MSVS2005_ERROR
#endif
#include <intrin.h>
#if _MSC_VER == 1400
#undef _interlockedbittestandset
#undef _interlockedbittestandreset
#undef _interlockedbittestandset64
#undef _interlockedbittestandreset64
#endif

//Ensure these methods are flagged as intrinsics, to avoid linking errors.
#ifdef _interlockedbittestandset
#pragma intrinsic(_interlockedbittestandset)
#endif
#ifdef _interlockedbittestandreset
#pragma intrinsic(_interlockedbittestandreset)
#endif
#ifdef _InterlockedCompareExchange
#pragma intrinsic(_InterlockedCompareExchange)
#endif
#ifdef _InterlockedCompareExchange_acq
#pragma intrinsic(_InterlockedCompareExchange_acq)
#endif
#ifdef _InterlockedCompareExchange_rel
#pragma intrinsic(_InterlockedCompareExchange_rel)
#endif
#ifdef _ReadBarrier
#pragma intrinsic(_ReadBarrier)
#endif
#ifdef _WriteBarrier
#pragma intrinsic(_WriteBarrier)
#endif
#ifdef _ReadWriteBarrier
#pragma intrinsic(_ReadWriteBarrier)
#endif

//Note that we need to use the align directive here to guarantee that our interlocked
//types will be correctly aligned to a memory boundary. All the interlocked memory
//functions require this alignment, otherwise the result is undefined.
typedef long InterlockedVar32;
typedef __int64 InterlockedVar64;
typedef __declspec(align(32)) InterlockedVar32 InterlockedVarAligned32;
typedef __declspec(align(64)) InterlockedVar64 InterlockedVarAligned64;

inline unsigned char PerformanceInterlockedBitTestAndSet(InterlockedVar32 volatile *a, InterlockedVar32 b)
{
	return _interlockedbittestandset(a, b);
}

inline unsigned char PerformanceInterlockedBitTestAndReset(InterlockedVar32 volatile *a, InterlockedVar32 b)
{
	return _interlockedbittestandreset(a, b);
}

inline long PerformanceInterlockedCompareExchangeAcquire(InterlockedVar32 volatile *destination, InterlockedVar32 exchange, InterlockedVar32 comperand)
{
#if defined(_InterlockedCompareExchange_acq)
	return _InterlockedCompareExchange_acq(destination, exchange, comperand);
#elif defined(_InterlockedCompareExchange)
	return _InterlockedCompareExchange(destination, exchange, comperand);
#else
	return InterlockedCompareExchange(destination, exchange, comperand);
#endif
}

inline long PerformanceInterlockedCompareExchangeRelease(InterlockedVar32 volatile *destination, InterlockedVar32 exchange, InterlockedVar32 comperand)
{
#if defined(_InterlockedCompareExchange_rel)
	return _InterlockedCompareExchange_rel(destination, exchange, comperand);
#elif defined(_InterlockedCompareExchange)
	return _InterlockedCompareExchange(destination, exchange, comperand);
#else
	return InterlockedCompareExchange(destination, exchange, comperand);
#endif
}

#endif
