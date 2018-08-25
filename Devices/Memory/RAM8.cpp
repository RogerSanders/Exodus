#include "RAM8.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RAM8::RAM8(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:RAMBase(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM8::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = ReadArrayValue(location);
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM8::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	WriteArrayValueWithLockCheckAndRollback(location, (unsigned char)data.GetData());
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RAM8::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = ReadArrayValue(location);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM8::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	WriteArrayValue(location, (unsigned char)data.GetData());
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int RAM8::ReadMemoryEntry(unsigned int location) const
{
	return ReadArrayValue(location);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM8::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	WriteArrayValue(location, (unsigned char)data);
}
