#ifndef __RAMBASE_H__
#define __RAMBASE_H__
#include "MemoryWrite.h"
#include <map>
#include <mutex>

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
	inline T ReadArrayValue(unsigned int arrayEntryPos) const;
	inline void WriteArrayValue(unsigned int arrayEntryPos, T newValue);
	inline void WriteArrayValueWithLockCheckAndRollback(unsigned int arrayEntryPos, T newValue);

	// Memory location functions
	inline unsigned int LimitLocationToMemorySize(unsigned int location) const;

private:
	// Memory location functions
	unsigned int LimitMemoryLocationToMemorySizePowerOfTwo(unsigned int location) const;
	unsigned int LimitMemoryLocationToMemorySizeNonPowerOfTwo(unsigned int location) const;

private:
	bool _initialMemoryDataSpecified;
	bool _repeatInitialMemoryData;
	std::vector<T> _initialMemoryData;
	bool _dataIsPersistent;

	unsigned int _memoryArraySize;
	unsigned int _memoryArraySizeMask;
	unsigned int (RAMBase::*_memoryLimitFunction)(unsigned int) const;

	T* _memoryArray;
	bool* _memoryLockedArray;
	std::unordered_map<unsigned int, T> _buffer;
	mutable std::mutex _bufferMutex;
};

#include "RAMBase.inl"
#endif
