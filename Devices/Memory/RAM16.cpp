#include "RAM16.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RAM16::RAM16(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:RAMBase(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	WriteArrayValueWithLockCheckAndRollback(location % memoryArraySize, (unsigned short)data.GetData());
	return true;
}

//----------------------------------------------------------------------------------------
void RAM16::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void RAM16::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	memoryArray[location % memoryArraySize] = (unsigned short)data.GetData();
}

//----------------------------------------------------------------------------------------
//Debug memory access functions
//----------------------------------------------------------------------------------------
unsigned int RAM16::ReadMemoryEntry(unsigned int location) const
{
	return memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void RAM16::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	memoryArray[location % memoryArraySize] = (unsigned short)data;
}
