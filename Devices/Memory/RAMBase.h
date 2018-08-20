#ifndef __RAMBASE_H__
#define __RAMBASE_H__
#include "MemoryWrite.h"
#include <vector>
#include <map>

template<class T>
class RAMBase :public MemoryWrite
{
public:
	// Constructors
	inline RAMBase(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	virtual ~RAMBase();

	// Initialization functions
	virtual bool Construct(IHierarchicalStorageNode& node);
	virtual void Initialize();

	// Memory size functions
	virtual unsigned int GetMemoryEntrySizeInBytes() const;

	// Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	// Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsAddressLocked(unsigned int location) const;

	// Savestate functions
	virtual void LoadState(IHierarchicalStorageNode& node);
	virtual void SaveState(IHierarchicalStorageNode& node) const;
	virtual void LoadPersistentState(IHierarchicalStorageNode& node);
	virtual void SavePersistentState(IHierarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHierarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHierarchicalStorageNode& node) const;

protected:
	// Access helper functions
	inline void WriteArrayValueWithLockCheckAndRollback(unsigned int arrayEntryPos, T newValue);

protected:
	typedef std::map<unsigned int, T> MemoryAccessBuffer;
	typedef std::pair<unsigned int, T> MemoryAccessBufferEntry;

	unsigned int _memoryArraySize;
	T* _memoryArray;
	bool* _memoryLockedArray;
	MemoryAccessBuffer _buffer;

private:
	bool _initialMemoryDataSpecified;
	bool _repeatInitialMemoryData;
	std::vector<T> _initialMemoryData;
	bool _dataIsPersistent;
};

#include "RAMBase.inl"
#endif
