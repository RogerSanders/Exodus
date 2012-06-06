#include "ROM.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ROM::ROM(const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryRead(L"ROM", ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
bool ROM::Construct(IHeirarchicalStorageNode& node)
{
	bool result = MemoryRead::Construct(node);

	//Validate that ROM data has been provided
	if(!node.GetBinaryDataPresent())
	{
		return false;
	}

	//Obtain the stream for our binary data
	Stream::IStream& dataStream = node.GetBinaryDataBufferStream();
	dataStream.SetStreamPos(0);

	//Set the size of our internal memory
	//##TODO## Consider failing the construction process if the ROM data is empty
	if(GetInterfaceSize() <= 0)
	{
		unsigned int interfaceSize = (unsigned int)dataStream.Size();
		if(interfaceSize <= 0)
		{
			interfaceSize = 1;
		}
		SetInterfaceSize(interfaceSize);
	}
	memory.resize(GetInterfaceSize());

	//Read in the ROM data
	unsigned int bytesToRead = ((unsigned int)memory.size() < (unsigned int)dataStream.Size())? (unsigned int)memory.size(): (unsigned int)dataStream.Size();
	if(!dataStream.ReadData(&memory[0], bytesToRead))
	{
		return false;
	}

	return result;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void ROM::Initialize()
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory[(location + i) % memory.size()]);
	}
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
void ROM::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory[(location + i) % memory.size()]);
	}
}

//----------------------------------------------------------------------------------------
void ROM::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		memory[(location + i) % memory.size()] = data.GetByte((dataByteSize - 1) - i);
	}
}
