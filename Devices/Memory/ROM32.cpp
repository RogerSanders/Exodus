#include "ROM32.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ROM32::ROM32(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:ROMBase(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM32::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = _memoryArray[LimitLocationToMemorySize(location)];
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM32::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ROM32::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = _memoryArray[LimitLocationToMemorySize(location)];
}

//----------------------------------------------------------------------------------------------------------------------
void ROM32::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	_memoryArray[LimitLocationToMemorySize(location)] = (unsigned int)data.GetData();
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ROM32::ReadMemoryEntry(unsigned int location) const
{
	return _memoryArray[LimitLocationToMemorySize(location)];
}

//----------------------------------------------------------------------------------------------------------------------
void ROM32::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	_memoryArray[LimitLocationToMemorySize(location)] = data;
}
