#include "RAM32.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RAM32::RAM32(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:RAMBase(implementationName, instanceName, moduleID)
{}

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM32::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	data = _memoryArray[location % _memoryArraySize];
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM32::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	WriteArrayValueWithLockCheckAndRollback(location % _memoryArraySize, (unsigned int)data.GetData());
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RAM32::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	data = _memoryArray[location % _memoryArraySize];
}

//----------------------------------------------------------------------------------------------------------------------
void RAM32::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	_memoryArray[location % _memoryArraySize] = (unsigned int)data.GetData();
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int RAM32::ReadMemoryEntry(unsigned int location) const
{
	return _memoryArray[location % _memoryArraySize];
}

//----------------------------------------------------------------------------------------------------------------------
void RAM32::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	_memoryArray[location % _memoryArraySize] = data;
}
