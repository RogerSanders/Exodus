#include "MemoryBarrier.h"
#include "WindowFunctions/WindowFunctions.pkg"
//#include <intrin.h>

//----------------------------------------------------------------------------------------
void SafeMemoryBarrierRead()
{
//	_ReadBarrier();
	MemoryBarrier();
}

//----------------------------------------------------------------------------------------
void SafeMemoryBarrierWrite()
{
//	_WriteBarrier();
	MemoryBarrier();
}

//----------------------------------------------------------------------------------------
void SafeMemoryBarrierReadWrite()
{
//	_ReadWriteBarrier();
	MemoryBarrier();
}
