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

	//If embedded ROM data has been specified, attempt to load it now.
	if(node.GetBinaryDataPresent())
	{
		//Obtain the stream for our binary data
		Stream::IStream& dataStream = node.GetBinaryDataBufferStream();
		dataStream.SetStreamPos(0);

		//Set the size of our internal memory
		if(GetInterfaceSize() <= 0)
		{
			unsigned int interfaceSize = (unsigned int)dataStream.Size();
			if(interfaceSize <= 0)
			{
				//If the embedded ROM data is empty, return false.
				return false;
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
	}
	else
	{
		//If no embedded ROM data has been provided, ensure that a valid interface size
		//has been specified, and set the size of our internal memory.
		unsigned int interfaceSize = GetInterfaceSize();
		result = (interfaceSize > 0);
		memory.resize(interfaceSize);
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
