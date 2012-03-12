#ifndef __MEMORYWRITE_H__
#define __MEMORYWRITE_H__
#include "MemoryRead.h"

class MemoryWrite :public MemoryRead
{
public:
	//Constructors
	MemoryWrite(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID);
};

#endif
