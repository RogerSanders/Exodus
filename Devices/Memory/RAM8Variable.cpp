#include "RAM8Variable.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RAM8Variable::RAM8Variable(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:RAMBase(implementationName, instanceName, moduleID)
{ }

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
			data.SetByteFromTopDown(i, ReadArrayValue(location + i));
		}
		break;}
	case 1:
		data = ReadArrayValue(location);
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)ReadArrayValue(baseLocation) << (arrayEntryByteSize * Data::BitsPerByte)) | (unsigned int)ReadArrayValue(baseLocation + 1);
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)ReadArrayValue(baseLocation) << (arrayEntryByteSize * 3 * Data::BitsPerByte)) | ((unsigned int)ReadArrayValue(baseLocation + 1) << (arrayEntryByteSize * 2 * Data::BitsPerByte)) | ((unsigned int)ReadArrayValue(baseLocation + 2) << (arrayEntryByteSize * 1 * Data::BitsPerByte)) | (unsigned int)ReadArrayValue(baseLocation + 3);
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
			WriteArrayValueWithLockCheckAndRollback(location + i, data.GetByteFromTopDown(i));
		}
		break;}
	case 1:
		WriteArrayValueWithLockCheckAndRollback(location, (unsigned char)data.GetData());
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValueWithLockCheckAndRollback(baseLocation, data.GetByteFromTopDown(0));
		WriteArrayValueWithLockCheckAndRollback(baseLocation + 1, data.GetByteFromTopDown(1));
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValueWithLockCheckAndRollback(baseLocation, data.GetByteFromTopDown(0));
		WriteArrayValueWithLockCheckAndRollback(baseLocation + 1, data.GetByteFromTopDown(1));
		WriteArrayValueWithLockCheckAndRollback(baseLocation + 2, data.GetByteFromTopDown(2));
		WriteArrayValueWithLockCheckAndRollback(baseLocation + 3, data.GetByteFromTopDown(3));
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
			WriteArrayValue(location + i, data.GetByteFromTopDown(i));
		}
		break;}
	case 1:
		WriteArrayValue(location, (unsigned char)data.GetData());
		break;
	case 2:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValue(baseLocation, data.GetByteFromTopDown(0));
		WriteArrayValue(baseLocation + 1, data.GetByteFromTopDown(1));
		break;}
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValue(baseLocation, data.GetByteFromTopDown(0));
		WriteArrayValue(baseLocation + 1, data.GetByteFromTopDown(1));
		WriteArrayValue(baseLocation + 2, data.GetByteFromTopDown(2));
		WriteArrayValue(baseLocation + 3, data.GetByteFromTopDown(3));
		break;}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int RAM8Variable::ReadMemoryEntry(unsigned int location) const
{
	return ReadArrayValue(location);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM8Variable::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	WriteArrayValue(location, (unsigned char)data);
}
