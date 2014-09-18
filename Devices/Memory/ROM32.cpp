#include "ROM32.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ROM32::ROM32(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:ROMBase(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM32::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM32::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
void ROM32::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void ROM32::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	memoryArray[location % memoryArraySize] = (unsigned int)data.GetData();
}

//----------------------------------------------------------------------------------------
//Debug memory access functions
//----------------------------------------------------------------------------------------
unsigned int ROM32::ReadMemoryEntry(unsigned int location) const
{
	return memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void ROM32::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	memoryArray[location % memoryArraySize] = data;
}
