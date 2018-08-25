#include "RAM16.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RAM16::RAM16(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:RAMBase(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = ReadArrayValue(location);
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(location), (unsigned short)data.GetData());
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RAM16::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = ReadArrayValue(location);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM16::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	WriteArrayValue(location, (unsigned short)data.GetData());
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int RAM16::ReadMemoryEntry(unsigned int location) const
{
	return ReadArrayValue(location);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM16::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	WriteArrayValue(location, (unsigned short)data);
}
