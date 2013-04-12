#ifndef __RAM_H__
#define __RAM_H__
#include "MemoryWrite.h"
#include <vector>
#include <map>

class RAM :public MemoryWrite
{
public:
	//Constructors
	RAM(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual bool Construct(IHeirarchicalStorageNode& node);

	//Initialization functions
	virtual void Initialize();

	//Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext);
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);

	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsByteLocked(unsigned int location) const;

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;
	virtual void LoadPersistentState(IHeirarchicalStorageNode& node);
	virtual void SavePersistentState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

private:
	typedef std::map<unsigned int, unsigned char> MemoryAccessBuffer;
	typedef std::pair<unsigned int, unsigned char> MemoryAccessBufferEntry;

	MemoryAccessBuffer buffer;
	std::vector<unsigned char> memory;
	std::vector<bool> memoryLocked;
	bool initialMemoryDataSpecified;
	bool repeatInitialMemoryData;
	std::vector<unsigned char> initialMemoryData;
	bool dataIsPersistent;
};

#endif
