#include "ROM.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ROM::ROM(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryRead(aimplementationName, ainstanceName, amoduleID)
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

		//If no interface size has been manually specified, set the size of our internal
		//memory based on the size of the specified embedded ROM data.
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

		//Resize the internal memory array based on the specified interface size, and
		//initialize all elements to 0.
		memory.assign(GetInterfaceSize(), 0);

		//Read the RepeatData attribute if specified
		bool repeatData = false;
		IHeirarchicalStorageAttribute* repeatDataAttribute = node.GetAttribute(L"RepeatData");
		if(repeatDataAttribute != 0)
		{
			repeatData = repeatDataAttribute->ExtractValue<bool>();
		}

		//Read in the ROM data
		unsigned int bytesInDataStream = (unsigned int)dataStream.Size();
		unsigned int memoryArraySize = (unsigned int)memory.size();
		unsigned int bytesToRead = (memoryArraySize < bytesInDataStream)? memoryArraySize: bytesInDataStream;
		if(!dataStream.ReadData(&memory[0], bytesToRead))
		{
			return false;
		}

		//If the data string has been set to repeat until the end of the memory block is
		//reached, fill out the remainder of the memory block now.
		if(repeatData)
		{
			for(unsigned int i = bytesToRead; i < memoryArraySize; ++i)
			{
				unsigned int originalDataIndex = i % bytesInDataStream;
				memory[i] = memory[originalDataIndex];
			}
		}
	}
	else
	{
		//If no embedded ROM data has been provided, ensure that a valid interface size
		//has been specified, and set the size of our internal memory.
		unsigned int interfaceSize = GetInterfaceSize();
		result = (interfaceSize > 0);
		memory.assign(interfaceSize, 0);
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
