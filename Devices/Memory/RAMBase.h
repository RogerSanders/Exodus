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
	virtual bool Construct(IHierarchicalStorageNode& node);
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
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadPersistentState(IHierarchicalStorageNode& node);
	virtual void SavePersistentState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

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
