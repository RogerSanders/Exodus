#include "ReferenceCounter.h"

//----------------------------------------------------------------------------------------
ReferenceCounterType ReferenceCounterIncrement(volatile ReferenceCounterType& referenceCounter)
{
	ReferenceCounterType referenceCounterOriginal;
	ReferenceCounterType referenceCounterModified;
	do
	{
		referenceCounterOriginal = referenceCounter;
		referenceCounterModified = referenceCounterOriginal + 1;
	}
	while (InterlockedCompareExchange(&referenceCounter, referenceCounterModified, referenceCounterOriginal) != referenceCounterOriginal);
	return referenceCounterModified;
}

//----------------------------------------------------------------------------------------
ReferenceCounterType ReferenceCounterDecrement(volatile ReferenceCounterType& referenceCounter)
{
	ReferenceCounterType referenceCounterOriginal;
	ReferenceCounterType referenceCounterModified;
	do
	{
		referenceCounterOriginal = referenceCounter;
		referenceCounterModified = referenceCounterOriginal - 1;
	}
	while (InterlockedCompareExchange(&referenceCounter, referenceCounterModified, referenceCounterOriginal) != referenceCounterOriginal);
	return referenceCounterModified;
}
