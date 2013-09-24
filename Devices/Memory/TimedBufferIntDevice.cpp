#include "TimedBufferIntDevice.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
TimedBufferIntDevice::TimedBufferIntDevice(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(aimplementationName, ainstanceName, amoduleID), initialMemoryDataSpecified(false), repeatInitialMemoryData(false)
{}

//----------------------------------------------------------------------------------------
bool TimedBufferIntDevice::Construct(IHeirarchicalStorageNode& node)
{
	//Call the base class Construct method
	if(!MemoryWrite::Construct(node))
	{
		return false;
	}

	//Validate the specified interface size
	if(GetMemoryEntryCount() <= 0)
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

	//Resize the internal memory array based on the specified interface size
	bufferShell.Resize(GetMemoryEntryCount(), keepLatestBufferCopy);

	//If initial RAM state data has been specified, attempt to load it now.
	if(node.GetBinaryDataPresent())
	{
		//Flag that initial memory data has been specified
		initialMemoryDataSpecified = true;

		//Obtain the stream for our binary data
		Stream::IStream& dataStream = node.GetBinaryDataBufferStream();
		dataStream.SetStreamPos(0);

		//Read the RepeatData attribute if specified
		IHeirarchicalStorageAttribute* repeatDataAttribute = node.GetAttribute(L"RepeatData");
		if(repeatDataAttribute != 0)
		{
			repeatInitialMemoryData = repeatDataAttribute->ExtractValue<bool>();
		}

		//Read in the initial memory data
		unsigned int bytesInDataStream = (unsigned int)dataStream.Size();
		unsigned int memoryArraySize = GetMemoryEntryCount();
		unsigned int bytesToRead = (memoryArraySize < bytesInDataStream)? memoryArraySize: bytesInDataStream;
		initialMemoryData.resize(bytesInDataStream);
		if(!dataStream.ReadData(&initialMemoryData[0], bytesToRead))
		{
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Memory size functions
//----------------------------------------------------------------------------------------
unsigned int TimedBufferIntDevice::GetMemoryEntrySizeInBytes() const
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::Initialize()
{
	//Initialize the memory buffer
	bufferShell.Initialize();
	for(unsigned int i = 0; i < GetMemoryEntryCount(); ++i)
	{
		if(!IsAddressLocked(i))
		{
			unsigned char initialValue = 0;
			if(initialMemoryDataSpecified && (repeatInitialMemoryData || (i < (unsigned int)initialMemoryData.size())))
			{
				unsigned int initialMemoryDataIndex = (i % (unsigned int)initialMemoryData.size());
				initialValue = initialMemoryData[initialMemoryDataIndex];
			}
			bufferShell.WriteLatest(i, initialValue);
		}
	}
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int memorySize = GetMemoryEntryCount();
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByteFromTopDown(i, bufferShell.ReadLatest((location + i) % memorySize));
	}
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int memorySize = GetMemoryEntryCount();
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		bufferShell.WriteLatest((location + i) % memorySize, data.GetByteFromTopDown(i));
	}
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::LoadState(IHeirarchicalStorageNode& node)
{
	bufferShell.LoadState(node);

	MemoryWrite::LoadState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::SaveState(IHeirarchicalStorageNode& node) const
{
	bufferShell.SaveState(node, GetFullyQualifiedDeviceInstanceName());

	MemoryWrite::SaveState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::LoadDebuggerState(IHeirarchicalStorageNode& node)
{
	bufferShell.LoadDebuggerState(node);

	MemoryWrite::LoadDebuggerState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::SaveDebuggerState(IHeirarchicalStorageNode& node) const
{
	bufferShell.SaveDebuggerState(node, GetFullyQualifiedDeviceInstanceName());

	MemoryWrite::SaveDebuggerState(node);
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
bool TimedBufferIntDevice::IsAddressLocked(unsigned int location) const
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
