#include "ROM32Variable.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ROM32Variable::ROM32Variable(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:ROM32(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM32Variable::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 4;
	switch(interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while(currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToExtractFromEntry = arrayEntryByteSize - ((location + currentDataByte) % arrayEntryByteSize);
			unsigned int bytesToExtractFromEntry = ((arrayEntryByteSize - firstByteOffsetToExtractFromEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - firstByteOffsetToExtractFromEntry): (dataByteSize - currentDataByte);
			for(unsigned int i = firstByteOffsetToExtractFromEntry; i < bytesToExtractFromEntry; ++i)
			{
				data.SetByteFromTopDown(currentDataByte++, (unsigned char)(memoryArray[baseLocation % memoryArraySize] >> (((arrayEntryByteSize - 1) - i) * Data::bitsPerByte)));
			}
		}
		break;}
	case 1:
	case 2:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::bitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::bitsPerByte * interfaceNumber)) - 1;
		data = ((unsigned int)memoryArray[baseLocation % memoryArraySize] >> dataShiftCount) & dataBitMask;
		break;}
	case 4:
		data = memoryArray[location % memoryArraySize];
		break;
	}
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult ROM32Variable::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	return true;
}

//----------------------------------------------------------------------------------------
void ROM32Variable::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	ReadInterface(interfaceNumber, location, data, caller, 0, accessContext);
}

//----------------------------------------------------------------------------------------
void ROM32Variable::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	static const unsigned int arrayEntryByteSize = 4;
	switch(interfaceNumber)
	{
	default:
	case 0:{
		unsigned int dataByteSize = data.GetByteSize();
		unsigned int currentDataByte = 0;
		while(currentDataByte < dataByteSize)
		{
			unsigned int baseLocation = (location + currentDataByte) / arrayEntryByteSize;
			unsigned int firstByteOffsetToWriteToEntry = arrayEntryByteSize - ((location + currentDataByte) % arrayEntryByteSize);
			unsigned int bytesToWriteToEntry = ((arrayEntryByteSize - firstByteOffsetToWriteToEntry) <= (dataByteSize - currentDataByte))? (arrayEntryByteSize - firstByteOffsetToWriteToEntry): (dataByteSize - currentDataByte);
			Data memoryEntry(arrayEntryByteSize * Data::bitsPerByte, memoryArray[baseLocation % memoryArraySize]);
			for(unsigned int i = firstByteOffsetToWriteToEntry; i < bytesToWriteToEntry; ++i)
			{
				memoryEntry.SetByteFromTopDown(i, data.GetByteFromTopDown(currentDataByte++));
			}
			memoryArray[baseLocation % memoryArraySize] = (unsigned int)memoryEntry.GetData();
		}
		break;}
	case 1:
	case 2:{
		unsigned int baseLocation = location / (interfaceNumber * arrayEntryByteSize);
		unsigned int dataShiftCount = (((arrayEntryByteSize / interfaceNumber) - 1) - (location % (arrayEntryByteSize / interfaceNumber))) * (Data::bitsPerByte * interfaceNumber);
		unsigned int dataBitMask = (1 << (Data::bitsPerByte * interfaceNumber)) - 1;
		memoryArray[baseLocation % memoryArraySize] = (memoryArray[baseLocation % memoryArraySize] & ~(dataBitMask << dataShiftCount)) | (data.GetData() << dataShiftCount);
		break;}
	case 4:
		memoryArray[location % memoryArraySize] = (unsigned int)data.GetData();
		break;
	}
}
