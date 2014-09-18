#include "ROM16.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ROM16::ROM16(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:ROMBase(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM16::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM16::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
void ROM16::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void ROM16::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	memoryArray[location % memoryArraySize] = (unsigned short)data.GetData();
}

//----------------------------------------------------------------------------------------
//Debug memory access functions
//----------------------------------------------------------------------------------------
unsigned int ROM16::ReadMemoryEntry(unsigned int location) const
{
	return memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void ROM16::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	memoryArray[location % memoryArraySize] = (unsigned short)data;
}
