#include "IMemory.h"
#ifndef __MEMORYREAD_H__
#define __MEMORYREAD_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "Device/Device.pkg"

//##TODO## Introduce a new interface for classes derived from this class, which
//provides a high-performance alternative to the Read/Write Transparent functions
//which specifically provide memory access for the debugger. The functions should
//work with arrays of data rather than individual values. In our TimedRAM class, we
//will implement the functions to work on the committed state rather than building
//information on the latest state. Actually, I'm not sure this will work, because
//isn't the "committed" state for a TimedRAM buffer defined as the baseline which
//has been advanced to, which is governed by our render thread?
class MemoryRead :public Device, public IMemory
{
public:
	//Constructors
	MemoryRead(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);

	//Interface version functions
	virtual unsigned int GetIMemoryVersion() const;

	//Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);

	//Memory size functions
	virtual unsigned int GetMemoryEntryCount() const;
	void SetMemoryEntryCount(unsigned int amemoryEntryCount);

protected:
	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsAddressLocked(unsigned int location) const;

private:
	//Memory size
	unsigned int memoryEntryCount;
};

#endif
