#include "ROM8Variable.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ROM8Variable::ROM8Variable(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:ROMBase(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM8Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 1;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		for (unsigned int i = 0; i < dataByteSize; ++i)
		{
			data.SetByteFromTopDown(i, _memoryArray[LimitLocationToMemorySize(location + i)]);
		}
		break;}
	case 1:
		data = _memoryArray[LimitLocationToMemorySize(location)];
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)_memoryArray[LimitLocationToMemorySize(baseLocation)] << (arrayEntryByteSize * Data::BitsPerByte)) | (unsigned int)_memoryArray[LimitLocationToMemorySize(baseLocation + 1)];
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)_memoryArray[LimitLocationToMemorySize(baseLocation)] << (arrayEntryByteSize * 3 * Data::BitsPerByte)) | ((unsigned int)_memoryArray[LimitLocationToMemorySize(baseLocation + 1)] << (arrayEntryByteSize * 2 * Data::BitsPerByte)) | ((unsigned int)_memoryArray[LimitLocationToMemorySize(baseLocation + 2)] << (arrayEntryByteSize * 1 * Data::BitsPerByte)) | (unsigned int)_memoryArray[LimitLocationToMemorySize(baseLocation + 3)];
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM8Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ROM8Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void ROM8Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 1;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		for (unsigned int i = 0; i < dataByteSize; ++i)
		{
			_memoryArray[LimitLocationToMemorySize(location + i)] = data.GetByteFromTopDown(i);
		}
		break;}
	case 1:
		_memoryArray[LimitLocationToMemorySize(location)] = (unsigned char)data.GetData();
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		_memoryArray[LimitLocationToMemorySize(baseLocation)] = data.GetByteFromTopDown(0);
		_memoryArray[LimitLocationToMemorySize(baseLocation + 1)] = data.GetByteFromTopDown(1);
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		_memoryArray[LimitLocationToMemorySize(baseLocation)] = data.GetByteFromTopDown(0);
		_memoryArray[LimitLocationToMemorySize(baseLocation + 1)] = data.GetByteFromTopDown(1);
		_memoryArray[LimitLocationToMemorySize(baseLocation + 2)] = data.GetByteFromTopDown(2);
		_memoryArray[LimitLocationToMemorySize(baseLocation + 3)] = data.GetByteFromTopDown(3);
		break;}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ROM8Variable::ReadMemoryEntry(unsigned int location) const
{
	return _memoryArray[LimitLocationToMemorySize(location)];
}

//----------------------------------------------------------------------------------------------------------------------
void ROM8Variable::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	_memoryArray[LimitLocationToMemorySize(location)] = (unsigned char)data;
}
