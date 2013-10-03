#include "RAM16Variable.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RAM16Variable::RAM16Variable(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:RAM16(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 2;
	switch(interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while(currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToExtractFromEntry = (location + currentDataByte) % arrayEntryByteSize;
			unsigned int lastByteOffsetToExtractFromEntry = ((arrayEntryByteSize - firstByteOffsetToExtractFromEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - 1): firstByteOffsetToExtractFromEntry + ((dataByteSize - 1) - currentDataByte);
			for(unsigned int i = firstByteOffsetToExtractFromEntry; i <= lastByteOffsetToExtractFromEntry; ++i)
			{
				data.SetByteFromTopDown(currentDataByte++, (unsigned char)(memoryArray[baseLocation % memoryArraySize] >> (((arrayEntryByteSize - 1) - i) * Data::bitsPerByte)));
			}
		}
		break;}
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::bitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::bitsPerByte * interfaceNumber)) - 1;
		data = ((unsigned int)memoryArray[baseLocation % memoryArraySize] >> dataShiftCount) & dataBitMask;
		break;}
	case 2:
		data = memoryArray[location % memoryArraySize];
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		data = ((unsigned int)memoryArray[baseLocation % memoryArraySize] << (arrayEntryByteSize * Data::bitsPerByte)) | (unsigned int)memoryArray[(baseLocation + 1) % memoryArraySize];
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM16Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 2;
	switch(interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while(currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToWriteToEntry = (location + currentDataByte) % arrayEntryByteSize;
			unsigned int lastByteOffsetToWriteToEntry = ((arrayEntryByteSize - firstByteOffsetToWriteToEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - 1): firstByteOffsetToWriteToEntry + ((dataByteSize - 1) - currentDataByte);
			Data memoryEntry(arrayEntryByteSize * Data::bitsPerByte, memoryArray[baseLocation % memoryArraySize]);
			for(unsigned int i = firstByteOffsetToWriteToEntry; i <= lastByteOffsetToWriteToEntry; ++i)
			{
				memoryEntry.SetByteFromTopDown(i, data.GetByteFromTopDown(currentDataByte++));
			}
			WriteArrayValueWithLockCheckAndRollback(baseLocation % memoryArraySize, (unsigned short)memoryEntry.GetData());
		}
		break;}
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::bitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::bitsPerByte * interfaceNumber)) - 1;
		WriteArrayValueWithLockCheckAndRollback(baseLocation % memoryArraySize, (memoryArray[baseLocation % memoryArraySize] & (unsigned short)~(dataBitMask << dataShiftCount)) | (unsigned short)(data.GetData() << dataShiftCount));
		break;}
	case 2:
		WriteArrayValueWithLockCheckAndRollback(location % memoryArraySize, (unsigned short)data.GetData());
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		WriteArrayValueWithLockCheckAndRollback(baseLocation % memoryArraySize, (unsigned short)data.GetDataSegment(((interfaceNumber / arrayEntryByteSize) - 1) * Data::bitsPerByte, arrayEntryByteSize * Data::bitsPerByte));
		WriteArrayValueWithLockCheckAndRollback((baseLocation + 1) % memoryArraySize, (unsigned short)data.GetDataSegment((((interfaceNumber / arrayEntryByteSize) - 1) - 1) * Data::bitsPerByte, arrayEntryByteSize * Data::bitsPerByte));
		break;}
	}
	return true;
}

//----------------------------------------------------------------------------------------
void RAM16Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------
void RAM16Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 2;
	switch(interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while(currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToWriteToEntry = (location + currentDataByte) % arrayEntryByteSize;
			unsigned int lastByteOffsetToWriteToEntry = ((arrayEntryByteSize - firstByteOffsetToWriteToEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - 1): firstByteOffsetToWriteToEntry + ((dataByteSize - 1) - currentDataByte);
			Data memoryEntry(arrayEntryByteSize * Data::bitsPerByte, memoryArray[baseLocation % memoryArraySize]);
			for(unsigned int i = firstByteOffsetToWriteToEntry; i <= lastByteOffsetToWriteToEntry; ++i)
			{
				memoryEntry.SetByteFromTopDown(i, data.GetByteFromTopDown(currentDataByte++));
			}
			memoryArray[baseLocation % memoryArraySize] = (unsigned short)memoryEntry.GetData();
		}
		break;}
	case 1:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::bitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::bitsPerByte * interfaceNumber)) - 1;
		memoryArray[baseLocation % memoryArraySize] = (memoryArray[baseLocation % memoryArraySize] & (unsigned short)~(dataBitMask << dataShiftCount)) | (unsigned short)(data.GetData() << dataShiftCount);
		break;}
	case 2:
		memoryArray[location % memoryArraySize] = (unsigned short)data.GetData();
		break;
	case 4:{
		unsigned int baseLocation = location * (interfaceNumber / arrayEntryByteSize);
		memoryArray[baseLocation % memoryArraySize] = (unsigned short)data.GetDataSegment(((interfaceNumber / arrayEntryByteSize) - 1) * Data::bitsPerByte, arrayEntryByteSize * Data::bitsPerByte);
		memoryArray[(baseLocation + 1) % memoryArraySize] = (unsigned short)data.GetDataSegment((((interfaceNumber / arrayEntryByteSize) - 1) - 1) * Data::bitsPerByte, arrayEntryByteSize * Data::bitsPerByte);
		break;}
	}
}
