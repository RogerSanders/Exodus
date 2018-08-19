#include "TimedBufferIntDevice.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
TimedBufferIntDevice::TimedBufferIntDevice(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:MemoryWrite(implementationName, instanceName, moduleID), _initialMemoryDataSpecified(false), _repeatInitialMemoryData(false)
{}

//----------------------------------------------------------------------------------------
bool TimedBufferIntDevice::Construct(IHierarchicalStorageNode& node)
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
	IHierarchicalStorageAttribute* keepLatestBufferCopyAttribute = node.GetAttribute(L"KeepLatestBufferCopy");
	if(keepLatestBufferCopyAttribute != 0)
	{
		keepLatestBufferCopy = keepLatestBufferCopyAttribute->ExtractValue<bool>();
	}

	//Resize the internal memory array based on the specified interface size
	_bufferShell.Resize(GetMemoryEntryCount(), keepLatestBufferCopy);

	//If initial RAM state data has been specified, attempt to load it now.
	if(node.GetBinaryDataPresent())
	{
		//Flag that initial memory data has been specified
		_initialMemoryDataSpecified = true;

		//Obtain the stream for our binary data
		Stream::IStream& dataStream = node.GetBinaryDataBufferStream();
		dataStream.SetStreamPos(0);

		//Read the RepeatData attribute if specified
		IHierarchicalStorageAttribute* repeatDataAttribute = node.GetAttribute(L"RepeatData");
		if(repeatDataAttribute != 0)
		{
			_repeatInitialMemoryData = repeatDataAttribute->ExtractValue<bool>();
		}

		//Read in the initial memory data
		unsigned int bytesInDataStream = (unsigned int)dataStream.Size();
		unsigned int memoryArraySize = GetMemoryEntryCount();
		unsigned int bytesToRead = (memoryArraySize < bytesInDataStream)? memoryArraySize: bytesInDataStream;
		_initialMemoryData.resize(bytesInDataStream);
		if(!dataStream.ReadData(&_initialMemoryData[0], bytesToRead))
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
	_bufferShell.Initialize();
	for(unsigned int i = 0; i < GetMemoryEntryCount(); ++i)
	{
		if(!IsAddressLocked(i))
		{
			unsigned char initialValue = 0;
			if(_initialMemoryDataSpecified && (_repeatInitialMemoryData || (i < (unsigned int)_initialMemoryData.size())))
			{
				unsigned int initialMemoryDataIndex = (i % (unsigned int)_initialMemoryData.size());
				initialValue = _initialMemoryData[initialMemoryDataIndex];
			}
			_bufferShell.WriteLatest(i, initialValue);
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
		data.SetByteFromTopDown(i, _bufferShell.ReadLatest((location + i) % memorySize));
	}
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int memorySize = GetMemoryEntryCount();
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		_bufferShell.WriteLatest((location + i) % memorySize, data.GetByteFromTopDown(i));
	}
}

//----------------------------------------------------------------------------------------
//Debug memory access functions
//----------------------------------------------------------------------------------------
unsigned int TimedBufferIntDevice::ReadMemoryEntry(unsigned int location) const
{
	unsigned int memorySize = GetMemoryEntryCount();
	return _bufferShell.ReadLatest(location % memorySize);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	unsigned int memorySize = GetMemoryEntryCount();
	_bufferShell.WriteLatest(location % memorySize, (unsigned char)data);
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::LoadState(IHierarchicalStorageNode& node)
{
	_bufferShell.LoadState(node);

	MemoryWrite::LoadState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::SaveState(IHierarchicalStorageNode& node) const
{
	_bufferShell.SaveState(node, GetFullyQualifiedDeviceInstanceName());

	MemoryWrite::SaveState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::LoadDebuggerState(IHierarchicalStorageNode& node)
{
	_bufferShell.LoadDebuggerState(node);

	MemoryWrite::LoadDebuggerState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferIntDevice::SaveDebuggerState(IHierarchicalStorageNode& node) const
{
	_bufferShell.SaveDebuggerState(node, GetFullyQualifiedDeviceInstanceName());

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
	_bufferShell.LockMemoryBlock(location, size, state);
}

//----------------------------------------------------------------------------------------
bool TimedBufferIntDevice::IsAddressLocked(unsigned int location) const
{
	return _bufferShell.IsByteLocked(location);
}

//----------------------------------------------------------------------------------------
//Buffer functions
//----------------------------------------------------------------------------------------
ITimedBufferInt* TimedBufferIntDevice::GetTimedBuffer()
{
	return &_bufferShell;
}
