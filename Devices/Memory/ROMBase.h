#ifndef __ROMBASE_H__
#define __ROMBASE_H__
#include "MemoryRead.h"

template<class T>
class ROMBase :public MemoryRead
{
public:
	// Constructors
	inline ROMBase(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	virtual ~ROMBase();

	// Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);

	// Memory size functions
	virtual unsigned int GetMemoryEntrySizeInBytes() const;

protected:
	// Memory location functions
	inline unsigned int LimitLocationToMemorySize(unsigned int location) const;

private:
	// Memory location functions
	unsigned int LimitMemoryLocationToMemorySizePowerOfTwo(unsigned int location) const;
	unsigned int LimitMemoryLocationToMemorySizeNonPowerOfTwo(unsigned int location) const;

protected:
	T* _memoryArray;

private:
	unsigned int _memoryArraySize;
	unsigned int _memoryArraySizeMask;
	unsigned int (ROMBase::*_memoryLimitFunction)(unsigned int) const;
};

#include "ROMBase.inl"
#endif
