#ifndef __MEMORYWRITE_H__
#define __MEMORYWRITE_H__
#include "MemoryRead.h"

class MemoryWrite :public MemoryRead
{
public:
	//Constructors
	MemoryWrite(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
};

#endif
