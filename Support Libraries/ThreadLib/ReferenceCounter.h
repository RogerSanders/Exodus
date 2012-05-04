/*
Description:
-These functions implement a very basic thread-safe counter increment/decrement system.
The specified reference counter will be either incremented or decremented, and the
resulting value of the reference counter after the operation will be returned. While the
functions themselves are thread-safe, and can be called concurrently from any number of
threads, EXTREME CARE must be exercised in their use. While the functions themselves can
handle the increment and decrement operations in a thread safe manner, and return the
correct result after that operation, no thread safety exists if you try and perform some
kind of operation based on the result. As soon as the function returns, the counter may
have changed again. The ONLY way the result of these operations can be used safely is if
the caller knows what the potential minimum or maximum values of the reference counter
are, and even then, only if the caller knows that once a maximum or minimum value is
reached, that the counter will definitely not change again.
*/
#ifndef __REFERENCECOUNTER_H__
#define __REFERENCECOUNTER_H__
#include "WindowFunctions/WindowFunctions.pkg"

typedef LONG ReferenceCounterType;
ReferenceCounterType ReferenceCounterIncrement(volatile ReferenceCounterType& referenceCounter);
ReferenceCounterType ReferenceCounterDecrement(volatile ReferenceCounterType& referenceCounter);

#endif
