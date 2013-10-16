#ifndef __PERFORMANCEMUTEX_H__
#define __PERFORMANCEMUTEX_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "InterlockedTypes.h"

//##NOTE## Tests have shown no significant performance improvements with these locking
//mechanisms compared to the standard locking mechanisms provided in the Boost.Threads
//library. The inlined asm method and InterlockedCompareExchange method appear to run
//at a similar speed. The performance difference between these methods and the locking
//system in boost in a real-world high collision scenario appears to be minimal.
struct PerformanceMutex
{
public:
	inline PerformanceMutex()
	:mutex(0)
	{}

	inline void Lock()
	{
		//Generate an explicit read/write barrier. This is not a memory barrier! This is a
		//compiler intrinsic to prevent instruction re-ordering. In this case, this
		//barrier will ensure that even if this function is inlined by the compiler, any
		//preceding instructions that work with shared memory will be performed before
		//this code is executed, ensuring that instructions are not re-ordered around the
		//mutex.
		_ReadWriteBarrier();

		//Enter a spin lock until the mutex can be acquired
		//while(InterlockedCompareExchangeAcquire(&mutex, 1, 0) != 0) {}
		while(PerformanceInterlockedBitTestAndSet(&mutex, 0)) {}

		//This final write barrier ensures that the compiler doesn't re-order our acquire
		//of the mutex after any following instructions.
		_WriteBarrier();
	}

	inline void Unlock()
	{
		//Generate an explicit read/write barrier. This is not a memory barrier! This is a
		//compiler intrinsic to prevent instruction re-ordering. In this case, this
		//barrier will ensure that even if this function is inlined by the compiler, any
		//preceding instructions that work with shared memory will be performed before
		//this code is executed, ensuring that instructions are not re-ordered around the
		//mutex.
		_ReadWriteBarrier();

		//Release the mutex
		//InterlockedCompareExchangeRelease(&mutex, 0, 1);
		PerformanceInterlockedBitTestAndReset(&mutex, 0);

		//This final write barrier ensures that the compiler doesn't re-order our release
		//of the mutex after any following instructions.
		_WriteBarrier();
	}

	//##NOTE## Inline asm isn't supported by x64 compiler
	//##NOTE## This doesn't generate a memory barrier! There is a special opcode to do
	//this. Add it into our code below.
	//void LockASM()
	//{
	//	__asm
	//	{
	//		mov     ebx, this
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
	//		mov     ebx, this
	//		mov     eax, 0

	//		xchg    eax, [ebx].mutex
	//		//mov    eax, [ebx].mutex
	//	}
	//}

private:
	volatile InterlockedVarAligned32 mutex;
};

#endif
