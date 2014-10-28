#ifndef __IMEMORY_H__
#define __IMEMORY_H__

class IMemory
{
public:
	//Interface version functions
	static inline unsigned int ThisIMemoryVersion() { return 1; }
	virtual unsigned int GetIMemoryVersion() const = 0;

	//Memory size functions
	virtual unsigned int GetMemoryEntrySizeInBytes() const = 0;
	virtual unsigned int GetMemoryEntryCount() const = 0;

	//Debug memory access functions
	virtual unsigned int ReadMemoryEntry(unsigned int location) const = 0;
	virtual void WriteMemoryEntry(unsigned int location, unsigned int data) = 0;

	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const = 0;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state) = 0;
	virtual bool IsAddressLocked(unsigned int location) const = 0;
};

#endif
