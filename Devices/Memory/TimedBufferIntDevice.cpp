#include "TimedBufferIntDevice.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
TimedBufferIntDevice::TimedBufferIntDevice(const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(L"TimedBufferIntDevice", ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
bool TimedBufferIntDevice::Construct(IHeirarchicalStorageNode& node)
{
	//Call the base class Construct method
	if(!MemoryWrite::Construct(node))
	{
		return false;
	}

	//Verify that a valid interface size has been specified
	if(GetInterfaceSize() <= 0)
	{
		return false;
	}

	//Read the KeepLatestBufferCopy attribute
	bool keepLatestBufferCopy = false;
	IHeirarchicalStorageAttribute* keepLatestBufferCopyAttribute = node.GetAttribute(L"KeepLatestBufferCopy");
	if(keepLatestBufferCopyAttribute != 0)
	{
		keepLatestBufferCopy = keepLatestBufferCopyAttribute->ExtractValue<bool>();
	}

	//Resize our internal buffer
	bufferShell.Resize(GetInterfaceSize(), keepLatestBufferCopy);

	return true;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int memorySize = GetInterfaceSize();
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, bufferShell.ReadLatest((location + i) % memorySize));
	}
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int memorySize = GetInterfaceSize();
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		bufferShell.WriteLatest((location + i) % memorySize, data.GetByte((dataByteSize - 1) - i));
	}
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::LoadState(IHeirarchicalStorageNode& node)
{
	bufferShell.LoadState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::GetState(IHeirarchicalStorageNode& node) const
{
	bufferShell.GetState(node, GetDeviceInstanceName());
}

//----------------------------------------------------------------------------------------
//Memory locking functions
//----------------------------------------------------------------------------------------
bool TimedBufferIntDevice::IsMemoryLockingSupported() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	bufferShell.LockMemoryBlock(location, size, state);
}

//----------------------------------------------------------------------------------------
bool TimedBufferIntDevice::IsByteLocked(unsigned int location) const
{
	return bufferShell.IsByteLocked(location);
}

//----------------------------------------------------------------------------------------
//Buffer functions
//----------------------------------------------------------------------------------------
ITimedBufferInt* TimedBufferIntDevice::GetTimedBuffer()
{
	return &bufferShell;
}
