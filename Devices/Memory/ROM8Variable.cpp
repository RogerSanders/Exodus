#include "ROM8Variable.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ROM8Variable::ROM8Variable(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:ROMBase(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM8Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 1;
	switch(interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		for(unsigned int i = 0; i < dataByteSize; ++i)
		{
			data.SetByteFromTopDown(i, memoryArray[(location + i) % memoryArraySize]);
		}
		break;}
	case 1:
		data = memoryArray[location % memoryArraySize];
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)memoryArray[baseLocation % memoryArraySize] << (arrayEntryByteSize * Data::bitsPerByte)) | (unsigned int)memoryArray[(baseLocation + 1) % memoryArraySize];
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)memoryArray[baseLocation % memoryArraySize] << (arrayEntryByteSize * 3 * Data::bitsPerByte)) | ((unsigned int)memoryArray[(baseLocation + 1) % memoryArraySize] << (arrayEntryByteSize * 2 * Data::bitsPerByte)) | ((unsigned int)memoryArray[(baseLocation + 2) % memoryArraySize] << (arrayEntryByteSize * 1 * Data::bitsPerByte)) | (unsigned int)memoryArray[(baseLocation + 3) % memoryArraySize];
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM8Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
void ROM8Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------
void ROM8Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 1;
	switch(interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		for(unsigned int i = 0; i < dataByteSize; ++i)
		{
			memoryArray[(location + i) % memoryArraySize] = data.GetByteFromTopDown(i);
		}
		break;}
	case 1:
		memoryArray[location % memoryArraySize] = (unsigned char)data.GetData();
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		memoryArray[baseLocation % memoryArraySize] = data.GetByteFromTopDown(0);
		memoryArray[(baseLocation + 1) % memoryArraySize] = data.GetByteFromTopDown(1);
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		memoryArray[baseLocation % memoryArraySize] = data.GetByteFromTopDown(0);
		memoryArray[(baseLocation + 1) % memoryArraySize] = data.GetByteFromTopDown(1);
		memoryArray[(baseLocation + 2) % memoryArraySize] = data.GetByteFromTopDown(2);
		memoryArray[(baseLocation + 3) % memoryArraySize] = data.GetByteFromTopDown(3);
		break;}
	}
}

//----------------------------------------------------------------------------------------
//Debug memory access functions
//----------------------------------------------------------------------------------------
unsigned int ROM8Variable::ReadMemoryEntry(unsigned int location) const
{
	return memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void ROM8Variable::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	memoryArray[location % memoryArraySize] = (unsigned char)data;
}
