#ifndef __PERFORMANCEMUTEX_H__
#define __PERFORMANCEMUTEX_H__
#include "WindowFunctions/WindowFunctions.pkg"
//#include <intrin.h>
//_ReadWriteBarrier;
//MemoryBarrier();

//##NOTE## Tests have shown no significant performance improvements with these locking
//mechanisms compared to the standard locking mechanisms provided in the Boost.Threads
//library. The inlined asm method and InterlockedCompareExchange method appear to run
//at a similar speed. The performance difference between these methods and the locking
//system in boost in a real-world high collision scenario appears to be minimal.
//##NOTE## Further reading suggests Lock() should use InterlockedCompareExchangeAcquire(),
//and Unlock() should use InterlockedCompareExchangeRelease() for better performance. This
//may optimize the memory barriers.
class PerformanceMutex
{
public:
	PerformanceMutex()
	:mutex(0)
	{}

	void Lock()
	{
		while(InterlockedCompareExchange(&mutex, 1, 0) != 0) {}
	}

	void Unlock()
	{
		InterlockedCompareExchange(&mutex, 0, 1);
	}

	//##NOTE## Inline asm isn't supported by x64 compiler
	//##NOTE## This doesn't generate a memory barrier! There is a special opcode to do
	//this. Add it into our code below.
	//void LockASM()
	//{
	//	__asm
	//	{
	//		mov		ebx, this
	//		mov     eax, 1

	//		spin_loop:
	//		xchg    eax, [ebx].mutex
	//		test    eax, eax
	//		jnz     spin_loop
	//	}
	//}

	//void UnlockASM()
	//{
	//	__asm
	//	{
	//		mov		ebx, this
	//		mov     eax, 0

	//		xchg    eax, [ebx].mutex
	//		//mov    eax, [ebx].mutex
	//	}
	//}

private:
	volatile LONG mutex;
};

#endif
