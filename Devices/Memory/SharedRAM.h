#ifndef __SHAREDRAM_H__
#define __SHAREDRAM_H__
#include "MemoryWrite.h"
#include <boost/thread/mutex.hpp>
#include <map>
#include <vector>

class SharedRAM :public MemoryWrite
{
public:
	//Constructors
	SharedRAM(const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual bool Construct(IHeirarchicalStorageNode& node);

	//Initialization functions
	virtual void Initialize();

	//Execute functions
	virtual void ExecuteRollback();
	virtual void ExecuteCommit();

	//Memory interface functions
	virtual IBusInterface::AccessResult ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime);
	virtual IBusInterface::AccessResult WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime);
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller);

	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsByteLocked(unsigned int location) const;

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void GetState(IHeirarchicalStorageNode& node) const;

private:
	//Rollback data
	struct MemoryWriteStatus
	{
		MemoryWriteStatus()
		{}
		MemoryWriteStatus(bool awritten, unsigned char adata, IDeviceContext* aauthor, double atimeslice)
		:written(awritten), shared(false), data(adata), author(aauthor), timeslice(atimeslice)
		{}

		bool written;
		bool shared;
		unsigned char data;
		IDeviceContext* author;
		double timeslice;
	};
	typedef std::map<unsigned int, MemoryWriteStatus> MemoryAccessBuffer;
	typedef std::pair<unsigned int, MemoryWriteStatus> MemoryAccessBufferEntry;

	boost::mutex accessLock;
	MemoryAccessBuffer buffer;
	std::vector<unsigned char> memory;
	std::vector<bool> memoryLocked;
};

#endif
