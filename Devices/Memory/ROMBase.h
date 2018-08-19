#ifndef __ROMBASE_H__
#define __ROMBASE_H__
#include "MemoryRead.h"

template<class T> class ROMBase :public MemoryRead
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
	unsigned int _memoryArraySize;
	T* _memoryArray;
};

#include "ROMBase.inl"
#endif
