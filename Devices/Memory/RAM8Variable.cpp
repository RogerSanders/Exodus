#include "RAM8Variable.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RAM8Variable::RAM8Variable(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:RAMBase(implementationName, instanceName, moduleID)
{}

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM8Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 1;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		for (unsigned int i = 0; i < dataByteSize; ++i)
		{
			data.SetByteFromTopDown(i, _memoryArray[(location + i) % _memoryArraySize]);
		}
		break;}
	case 1:
		data = _memoryArray[location % _memoryArraySize];
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)_memoryArray[baseLocation % _memoryArraySize] << (arrayEntryByteSize * Data::BitsPerByte)) | (unsigned int)_memoryArray[(baseLocation + 1) % _memoryArraySize];
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)_memoryArray[baseLocation % _memoryArraySize] << (arrayEntryByteSize * 3 * Data::BitsPerByte)) | ((unsigned int)_memoryArray[(baseLocation + 1) % _memoryArraySize] << (arrayEntryByteSize * 2 * Data::BitsPerByte)) | ((unsigned int)_memoryArray[(baseLocation + 2) % _memoryArraySize] << (arrayEntryByteSize * 1 * Data::BitsPerByte)) | (unsigned int)_memoryArray[(baseLocation + 3) % _memoryArraySize];
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM8Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 1;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		for (unsigned int i = 0; i < dataByteSize; ++i)
		{
			WriteArrayValueWithLockCheckAndRollback((location + i) % _memoryArraySize, data.GetByteFromTopDown(i));
		}
		break;}
	case 1:
		WriteArrayValueWithLockCheckAndRollback(location % _memoryArraySize, (unsigned char)data.GetData());
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValueWithLockCheckAndRollback(baseLocation % _memoryArraySize, data.GetByteFromTopDown(0));
		WriteArrayValueWithLockCheckAndRollback((baseLocation + 1) % _memoryArraySize, data.GetByteFromTopDown(1));
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValueWithLockCheckAndRollback(baseLocation % _memoryArraySize, data.GetByteFromTopDown(0));
		WriteArrayValueWithLockCheckAndRollback((baseLocation + 1) % _memoryArraySize, data.GetByteFromTopDown(1));
		WriteArrayValueWithLockCheckAndRollback((baseLocation + 2) % _memoryArraySize, data.GetByteFromTopDown(2));
		WriteArrayValueWithLockCheckAndRollback((baseLocation + 3) % _memoryArraySize, data.GetByteFromTopDown(3));
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RAM8Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM8Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 1;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		for (unsigned int i = 0; i < dataByteSize; ++i)
		{
			_memoryArray[(location + i) % _memoryArraySize] = data.GetByteFromTopDown(i);
		}
		break;}
	case 1:
		_memoryArray[location % _memoryArraySize] = (unsigned char)data.GetData();
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		_memoryArray[baseLocation % _memoryArraySize] = data.GetByteFromTopDown(0);
		_memoryArray[(baseLocation + 1) % _memoryArraySize] = data.GetByteFromTopDown(1);
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		_memoryArray[baseLocation % _memoryArraySize] = data.GetByteFromTopDown(0);
		_memoryArray[(baseLocation + 1) % _memoryArraySize] = data.GetByteFromTopDown(1);
		_memoryArray[(baseLocation + 2) % _memoryArraySize] = data.GetByteFromTopDown(2);
		_memoryArray[(baseLocation + 3) % _memoryArraySize] = data.GetByteFromTopDown(3);
		break;}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int RAM8Variable::ReadMemoryEntry(unsigned int location) const
{
	return _memoryArray[location % _memoryArraySize];
}

//----------------------------------------------------------------------------------------------------------------------
void RAM8Variable::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	_memoryArray[location % _memoryArraySize] = (unsigned char)data;
}
