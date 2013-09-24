#include "RAM32.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RAM32::RAM32(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:RAMBase(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM32::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM32::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	WriteArrayValueWithLockCheckAndRollback(location % memoryArraySize, (unsigned int)data.GetData());
	return true;
}

//----------------------------------------------------------------------------------------
void RAM32::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void RAM32::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	memoryArray[location % memoryArraySize] = (unsigned int)data.GetData();
}
