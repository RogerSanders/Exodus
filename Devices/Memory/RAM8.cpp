#include "RAM8.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RAM8::RAM8(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:RAMBase(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM8::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM8::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	WriteArrayValueWithLockCheckAndRollback(location % memoryArraySize, (unsigned char)data.GetData());
	return true;
}

//----------------------------------------------------------------------------------------
void RAM8::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = memoryArray[location % memoryArraySize];
}

//----------------------------------------------------------------------------------------
void RAM8::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	memoryArray[location % memoryArraySize] = (unsigned char)data.GetData();
}
