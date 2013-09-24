#ifndef __ROMBASE_H__
#define __ROMBASE_H__
#include "MemoryRead.h"

template<class T> class ROMBase :public MemoryRead
{
public:
	//Constructors
	inline ROMBase(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~ROMBase();

	//Initialization functions
	virtual bool Construct(IHeirarchicalStorageNode& node);

	//Memory size functions
	virtual unsigned int GetMemoryEntrySizeInBytes() const;

protected:
	unsigned int memoryArraySize;
	T* memoryArray;
};

#include "ROMBase.inl"
#endif
