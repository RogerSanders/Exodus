#ifndef __TIMEDBUFFERINTDEVICE_H__
#define __TIMEDBUFFERINTDEVICE_H__
#include "MemoryWrite.h"
#include "TimedBufferInt.h"
#include "TimedBuffers/TimedBuffers.pkg"

class TimedBufferIntDevice :public MemoryWrite, public ITimedBufferIntDevice
{
public:
	//Constructors
	TimedBufferIntDevice(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual bool Construct(IHeirarchicalStorageNode& node);

	//Initialization functions
	virtual void Initialize();

	//Memory interface functions
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext);

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void SaveState(IHeirarchicalStorageNode& node) const;
	virtual void LoadDebuggerState(IHeirarchicalStorageNode& node);
	virtual void SaveDebuggerState(IHeirarchicalStorageNode& node) const;

	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsByteLocked(unsigned int location) const;

	//Buffer functions
	ITimedBufferInt* GetTimedBuffer();

private:
	TimedBufferInt bufferShell;
	bool initialMemoryDataSpecified;
	bool repeatInitialMemoryData;
	std::vector<unsigned char> initialMemoryData;
};

#endif
