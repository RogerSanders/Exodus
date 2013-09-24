#ifndef __RAMBASE_H__
#define __RAMBASE_H__
#include "MemoryWrite.h"
#include <vector>
#include <map>

template<class T> class RAMBase :public MemoryWrite
{
public:
	//Constructors
	inline RAMBase(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual ~RAMBase();

	//Initialization functions
	virtual bool Construct(IHeirarchicalStorageNode& node);
	virtual void Initialize();

	//Memory size functions
	virtual unsigned int GetMemoryEntrySizeInBytes() const;

	//Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsAddressLocked(unsigned int location) const;

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;
	virtual void LoadPersistentState(IHeirarchicalStorageNode& node);
	virtual void SavePersistentState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

protected:
	//Access helper functions
	inline void WriteArrayValueWithLockCheckAndRollback(unsigned int arrayEntryPos, T newValue);

protected:
	typedef std::map<unsigned int, T> MemoryAccessBuffer;
	typedef std::pair<unsigned int, T> MemoryAccessBufferEntry;

	unsigned int memoryArraySize;
	T* memoryArray;
	bool* memoryLockedArray;
	MemoryAccessBuffer buffer;

private:
	bool initialMemoryDataSpecified;
	bool repeatInitialMemoryData;
	std::vector<T> initialMemoryData;
	bool dataIsPersistent;
};

#include "RAMBase.inl"
#endif
