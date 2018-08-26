#include "RAM16Variable.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RAM16Variable::RAM16Variable(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:RAMBase(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 2;
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
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		data = ((unsigned int)ReadArrayValue(baseLocation) >> dataShiftCount) & dataBitMask;
		break;}
	case 2:
		data = ReadArrayValue(location);
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)ReadArrayValue(baseLocation) << (arrayEntryByteSize * Data::BitsPerByte)) | (unsigned int)ReadArrayValue(baseLocation + 1);
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 2;
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
			WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(baseLocation), (unsigned short)memoryEntry.GetData());
		}
		break;}
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(baseLocation), (ReadArrayValue(baseLocation) & (unsigned short)~(dataBitMask << dataShiftCount)) | (unsigned short)(data.GetData() << dataShiftCount));
		break;}
	case 2:
		WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(location), (unsigned short)data.GetData());
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(baseLocation), (unsigned short)data.GetDataSegment(((interfaceNumber / arrayEntryByteSize) - 1) * Data::BitsPerByte, arrayEntryByteSize * Data::BitsPerByte));
		WriteArrayValueWithLockCheckAndRollback(LimitLocationToMemorySize(baseLocation + 1), (unsigned short)data.GetDataSegment((((interfaceNumber / arrayEntryByteSize) - 1) - 1) * Data::BitsPerByte, arrayEntryByteSize * Data::BitsPerByte));
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RAM16Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM16Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 2;
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
			WriteArrayValue(baseLocation, (unsigned short)memoryEntry.GetData());
		}
		break;}
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		WriteArrayValue(baseLocation, (ReadArrayValue(baseLocation) & (unsigned short)~(dataBitMask << dataShiftCount)) | (unsigned short)(data.GetData() << dataShiftCount));
		break;}
	case 2:
		WriteArrayValue(location, (unsigned short)data.GetData());
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValue(baseLocation, (unsigned short)data.GetDataSegment(((interfaceNumber / arrayEntryByteSize) - 1) * Data::BitsPerByte, arrayEntryByteSize * Data::BitsPerByte));
		WriteArrayValue(baseLocation + 1, (unsigned short)data.GetDataSegment((((interfaceNumber / arrayEntryByteSize) - 1) - 1) * Data::BitsPerByte, arrayEntryByteSize * Data::BitsPerByte));
		break;}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int RAM16Variable::ReadMemoryEntry(unsigned int location) const
{
	return ReadArrayValue(location);
}

//----------------------------------------------------------------------------------------------------------------------
void RAM16Variable::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	WriteArrayValue(location, (unsigned short)data);
}
