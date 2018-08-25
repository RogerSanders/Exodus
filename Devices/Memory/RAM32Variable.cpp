#include "RAM32Variable.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RAM32Variable::RAM32Variable(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:RAMBase(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM32Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 4;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while (currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToExtractFromEntry = (location + currentDataByte) % arrayEntryByteSize;
			unsigned int lastByteOffsetToExtractFromEntry = ((arrayEntryByteSize - firstByteOffsetToExtractFromEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - 1): firstByteOffsetToExtractFromEntry + ((dataByteSize - 1) - currentDataByte);
			for (unsigned int i = firstByteOffsetToExtractFromEntry; i <= lastByteOffsetToExtractFromEntry; ++i)
			{
				data.SetByteFromTopDown(currentDataByte++, (unsigned char)(ReadArrayValue(baseLocation) >> (((arrayEntryByteSize - 1) - i) * Data::BitsPerByte)));
			}
		}
		break;}
	case 1:
	case 2:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		data = (ReadArrayValue(baseLocation) >> dataShiftCount) & dataBitMask;
		break;}
	case 4:
		data = ReadArrayValue(location);
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM32Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 4;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while (currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToWriteToEntry = (location + currentDataByte) % arrayEntryByteSize;
			unsigned int lastByteOffsetToWriteToEntry = ((arrayEntryByteSize - firstByteOffsetToWriteToEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - 1): firstByteOffsetToWriteToEntry + ((dataByteSize - 1) - currentDataByte);
			Data memoryEntry(arrayEntryByteSize * Data::BitsPerByte, ReadArrayValue(baseLocation));
			for (unsigned int i = firstByteOffsetToWriteToEntry; i <= lastByteOffsetToWriteToEntry; ++i)
			{
				memoryEntry.SetByteFromTopDown(i, data.GetByteFromTopDown(currentDataByte++));
			}
			WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(baseLocation), memoryEntry.GetData());
		}
		break;}
	case 1:
	case 2:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(baseLocation), (ReadArrayValue(baseLocation) & ~(dataBitMask << dataShiftCount)) | (data.GetData() << dataShiftCount));
		break;}
	case 4:
		WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(location), data.GetData());
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RAM32Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM32Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 4;
	switch (interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while (currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToWriteToEntry = (location + currentDataByte) % arrayEntryByteSize;
			unsigned int lastByteOffsetToWriteToEntry = ((arrayEntryByteSize - firstByteOffsetToWriteToEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - 1): firstByteOffsetToWriteToEntry + ((dataByteSize - 1) - currentDataByte);
			Data memoryEntry(arrayEntryByteSize * Data::BitsPerByte, ReadArrayValue(baseLocation));
			for (unsigned int i = firstByteOffsetToWriteToEntry; i <= lastByteOffsetToWriteToEntry; ++i)
			{
				memoryEntry.SetByteFromTopDown(i, data.GetByteFromTopDown(currentDataByte++));
			}
			WriteArrayValue(baseLocation, memoryEntry.GetData());
		}
		break;}
	case 1:
	case 2:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		WriteArrayValue(baseLocation, (ReadArrayValue(baseLocation) & ~(dataBitMask << dataShiftCount)) | (data.GetData() << dataShiftCount));
		break;}
	case 4:
		WriteArrayValue(location, data.GetData());
		break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int RAM32Variable::ReadMemoryEntry(unsigned int location) const
{
	return ReadArrayValue(location);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM32Variable::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	WriteArrayValue(location, data);
}
