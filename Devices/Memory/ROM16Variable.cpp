#include "ROM16Variable.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ROM16Variable::ROM16Variable(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:ROMBase(implementationName, instanceName, moduleID)
{}

//----------------------------------------------------------------------------------------------------------------------
// Memory interface functions
//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM16Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
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
				data.SetByteFromTopDown(currentDataByte++, (unsigned char)(_memoryArray[baseLocation % _memoryArraySize] >> (((arrayEntryByteSize - 1) - i) * Data::BitsPerByte)));
			}
		}
		break;}
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		data = ((unsigned int)_memoryArray[baseLocation % _memoryArraySize] >> dataShiftCount) & dataBitMask;
		break;}
	case 2:
		data = _memoryArray[location % _memoryArraySize];
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)_memoryArray[baseLocation % _memoryArraySize] << (arrayEntryByteSize * Data::BitsPerByte)) | (unsigned int)_memoryArray[(baseLocation + 1) % _memoryArraySize];
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM16Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ROM16Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------------------------------------
void ROM16Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
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
			Data memoryEntry(arrayEntryByteSize * Data::BitsPerByte, _memoryArray[baseLocation % _memoryArraySize]);
			for (unsigned int i = firstByteOffsetToWriteToEntry; i <= lastByteOffsetToWriteToEntry; ++i)
			{
				memoryEntry.SetByteFromTopDown(i, data.GetByteFromTopDown(currentDataByte++));
			}
			_memoryArray[baseLocation % _memoryArraySize] = (unsigned short)memoryEntry.GetData();
		}
		break;}
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::BitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::BitsPerByte * interfaceNumber)) - 1;
		_memoryArray[baseLocation % _memoryArraySize] = (_memoryArray[baseLocation % _memoryArraySize] & (unsigned short)~(dataBitMask << dataShiftCount)) | (unsigned short)(data.GetData() << dataShiftCount);
		break;}
	case 2:
		_memoryArray[location % _memoryArraySize] = (unsigned short)data.GetData();
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		_memoryArray[baseLocation % _memoryArraySize] = (unsigned short)data.GetDataSegment(((interfaceNumber / arrayEntryByteSize) - 1) * Data::BitsPerByte, arrayEntryByteSize * Data::BitsPerByte);
		_memoryArray[(baseLocation + 1) % _memoryArraySize] = (unsigned short)data.GetDataSegment((((interfaceNumber / arrayEntryByteSize) - 1) - 1) * Data::BitsPerByte, arrayEntryByteSize * Data::BitsPerByte);
		break;}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Debug memory access functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ROM16Variable::ReadMemoryEntry(unsigned int location) const
{
	return _memoryArray[location % _memoryArraySize];
}

//----------------------------------------------------------------------------------------------------------------------
void ROM16Variable::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	_memoryArray[location % _memoryArraySize] = (unsigned short)data;
}
