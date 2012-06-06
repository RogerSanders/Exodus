#include "BusInterface.h"
#include "SystemInterface/SystemInterface.pkg"
#include "DataConversion/DataConversion.pkg"
#include "Debug/Debug.pkg"
#include <set>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
BusInterface::BusInterface()
:memoryInterfaceDefined(false), portInterfaceDefined(false), nextCELineID(1)
{}

//----------------------------------------------------------------------------------------
BusInterface::~BusInterface()
{
	//Delete all the list entries from the physical memory map
	for(unsigned int i = 0; i < physicalMemoryMap.size(); ++i)
	{
		delete physicalMemoryMap[i];
	}

	//Delete all the allocated MapEntry objects from the memory map
	for(unsigned int i = 0; i < memoryMap.size(); ++i)
	{
		delete memoryMap[i];
	}

	//Delete all the list entries from the physical port map
	for(unsigned int i = 0; i < physicalPortMap.size(); ++i)
	{
		delete physicalPortMap[i];
	}

	//Delete all the allocated MapEntry objects from the port map
	for(unsigned int i = 0; i < portMap.size(); ++i)
	{
		delete portMap[i];
	}
}

//----------------------------------------------------------------------------------------
bool BusInterface::Construct(const BusInterfaceParams& params)
{
	//Load the memory map parameters
	if(params.addressBusWidthDefined && params.dataBusWidthDefined)
	{
		memoryInterfaceDefined = true;
		addressBusWidth = params.addressBusWidth;
		addressBusMask = ((1 << addressBusWidth) - 1);
		dataBusWidth = params.dataBusWidth;

		if(params.usePhysicalMemoryMapDefined)
		{
			usePhysicalMemoryMap = params.usePhysicalMemoryMap;
		}
		else
		{
			usePhysicalMemoryMap = (addressBusWidth <= 24);
		}
		if(usePhysicalMemoryMap)
		{
			physicalMemoryMap.resize(1 << addressBusWidth, 0);
		}
	}

	//Load the port map parameters
	if(params.portAddressBusWidthDefined && params.portDataBusWidthDefined)
	{
		portInterfaceDefined = true;
		portAddressBusWidth = params.portAddressBusWidth;
		portAddressBusMask = ((1 << portAddressBusWidth) - 1);
		portDataBusWidth = params.portDataBusWidth;

		if(params.usePhysicalPortMapDefined)
		{
			usePhysicalPortMap = params.usePhysicalPortMap;
		}
		else
		{
			usePhysicalPortMap = (portAddressBusWidth <= 24);
		}
		if(usePhysicalPortMap)
		{
			physicalPortMap.resize(1 << portAddressBusWidth, 0);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::Construct(IHeirarchicalStorageNode& node)
{
	BusInterfaceParams params;

	//Load the memory map parameters
	IHeirarchicalStorageAttribute* addressBusWidthAttribute = node.GetAttribute(L"AddressBusWidth");
	if(addressBusWidthAttribute != 0)
	{
		params.addressBusWidthDefined = true;
		params.addressBusWidth = addressBusWidthAttribute->ExtractValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* dataBusWidthAttribute = node.GetAttribute(L"DataBusWidth");
	if(dataBusWidthAttribute != 0)
	{
		params.dataBusWidthDefined = true;
		params.dataBusWidth = dataBusWidthAttribute->ExtractValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* usePhysicalMemoryMapAttribute = node.GetAttribute(L"UsePhysicalMemoryMap");
	if(usePhysicalMemoryMapAttribute != 0)
	{
		params.usePhysicalMemoryMapDefined = true;
		params.usePhysicalMemoryMap = usePhysicalMemoryMapAttribute->ExtractValue<bool>();
	}

	//Load the port map parameters
	IHeirarchicalStorageAttribute* portAddressBusWidthAttribute = node.GetAttribute(L"PortAddressBusWidth");
	if(portAddressBusWidthAttribute != 0)
	{
		params.portAddressBusWidthDefined = true;
		params.portAddressBusWidth = portAddressBusWidthAttribute->ExtractValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* portDataBusWidthAttribute = node.GetAttribute(L"PortDataBusWidth");
	if(portDataBusWidthAttribute != 0)
	{
		params.portDataBusWidthDefined = true;
		params.portDataBusWidth = portDataBusWidthAttribute->ExtractValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* usePhysicalPortMapAttribute = node.GetAttribute(L"UsePhysicalPortMap");
	if(usePhysicalPortMapAttribute != 0)
	{
		params.usePhysicalPortMapDefined = true;
		params.usePhysicalPortMap = usePhysicalPortMapAttribute->ExtractValue<bool>();
	}

	//Try and construct the bus
	return Construct(params);
}

//----------------------------------------------------------------------------------------
//Unmapping functions
//----------------------------------------------------------------------------------------
void BusInterface::RemoveAllReferencesToDevice(IDevice* device)
{
	//Remove all references to this device from our internal structures
	UnmapMemoryForDevice(device);
	UnmapPortForDevice(device);
	UnmapLineForDevice(device);
}

//----------------------------------------------------------------------------------------
//Generic map entry functions
//----------------------------------------------------------------------------------------
bool BusInterface::BuildMapEntry(MapEntry& mapEntry, IDevice* device, const DeviceMappingParams& params, unsigned int busMappingAddressBusMask, unsigned int busMappingAddressBusWidth, unsigned int busMappingDataBusWidth, bool memoryMapping) const
{
	//Abort if we don't have all the required parameters
	if(!params.memoryMapSizeDefined || (!params.memoryMapBaseDefined && !params.addressLineFilterDefined))
	{
		return false;
	}

	//Create a new MapEntry object
	mapEntry.device = device;

	//Handle the MemoryMapSize parameter
	mapEntry.interfaceSize = params.memoryMapSize;

	//Handle an InterfaceOffset parameter
	if(params.interfaceOffsetDefined)
	{
		mapEntry.interfaceOffset = params.interfaceOffset;
	}

	//Handle an InterfaceNumber parameter
	if(params.interfaceNumberDefined)
	{
		mapEntry.interfaceNumber = params.interfaceNumber;
	}

	//Handle a MemoryMapBase parameter
	if(params.memoryMapBaseDefined)
	{
		mapEntry.address = params.memoryMapBase;
		mapEntry.addressMask = busMappingAddressBusMask;
	}

	//Handle an AddressLineFilter parameter
	if(params.addressLineFilterDefined)
	{
		std::wstring addressLineFilter = params.addressLineFilter;
		mapEntry.address = 0;
		mapEntry.addressMask = busMappingAddressBusMask;
		for(unsigned int i = 0; i < addressLineFilter.size(); ++i)
		{
			if((addressLineFilter[i] != L'0') && (addressLineFilter[i] != L'1') && (addressLineFilter[i] != L'?'))
			{
				continue;
			}

			mapEntry.address <<= 1;
			mapEntry.addressMask <<= 1;
			if(addressLineFilter[i] == L'1')
			{
				mapEntry.address |= 1;
			}
			else if(addressLineFilter[i] == L'?')
			{
				mapEntry.addressMask |= 1;
			}
		}
		mapEntry.addressMask = ~mapEntry.addressMask & busMappingAddressBusMask;
	}

	//Handle a CELineConditions parameter
	if(params.ceLineConditionsDefined)
	{
		//Tokenize ceLineConditionsString, and extract the names of individual CE lines to
		//check the state of, and the values to check against.
		std::wstring ceLineConditionsString = params.ceLineConditions;
		std::map<std::wstring, unsigned int> ceLineConditionMap;
		size_t nextConditionStringSearchPos = 0;
		bool sameLineDefinedTwice = false;
		bool done = false;
		while(!done)
		{
			//Skip any leading separators
			size_t lineNameStartPos = ceLineConditionsString.find_first_not_of(L"\t ,", nextConditionStringSearchPos);
			if(lineNameStartPos == ceLineConditionsString.npos)
			{
				done = true;
				continue;
			}

			//Find the end of the line name
			size_t lineNameEndPos = ceLineConditionsString.find_first_of(L"=\t ,", lineNameStartPos);
			if(lineNameEndPos == ceLineConditionsString.npos)
			{
				done = true;
				continue;
			}

			//Find the assignment operator
			size_t separatorPos = ceLineConditionsString.find_first_of(L'=', lineNameEndPos);
			if(separatorPos == ceLineConditionsString.npos)
			{
				done = true;
				continue;
			}

			//Find the start of the value
			size_t valueStartPos = ceLineConditionsString.find_first_not_of(L"=\t ,", separatorPos);
			if(valueStartPos == ceLineConditionsString.npos)
			{
				done = true;
				continue;
			}

			//Find the end of the value
			size_t valueEndPos = ceLineConditionsString.find_first_of(L"\t ,", valueStartPos);
			if(valueEndPos == ceLineConditionsString.npos)
			{
				done = true;
				valueEndPos = ceLineConditionsString.size();
			}

			//Set the search position for the next condition
			nextConditionStringSearchPos = valueEndPos;

			//Extract the line name and value
			std::wstring lineName = ceLineConditionsString.substr(lineNameStartPos, lineNameEndPos - lineNameStartPos);
			std::wstring valueString = ceLineConditionsString.substr(valueStartPos, valueEndPos - valueStartPos);

			//Convert the value from a string to an integer value
			unsigned int value = 0;
			StringToIntBase2(valueString, value);

			//Insert this CE line name/value pair into our list of CE line conditions
			if(!ceLineConditionMap.insert(std::pair<std::wstring, unsigned int>(lineName, value)).second)
			{
				//If this CE line has already been specified in the condition string, flag
				//an error.
				sameLineDefinedTwice = true;
			}
		}

		//If the same CE line was specified more than once in the CELineConditions string,
		//return false.
		if(sameLineDefinedTwice)
		{
			return false;
		}

		//Locate the referenced CE lines, and store the busLineID and condition value for
		//each line.
		bool foundAllCELines = true;
		for(std::map<std::wstring, unsigned int>::const_iterator ceLineConditionIterator = ceLineConditionMap.begin(); ceLineConditionIterator != ceLineConditionMap.end(); ++ceLineConditionIterator)
		{
			//Search for a defined CE line with the same name
			const CELineMap& targetCELineMap = memoryMapping? ceLineDefinitionsMemory: ceLineDefinitionsPort;
			CELineMap::const_iterator targetCELineMapIterator = targetCELineMap.begin();
			bool foundTargetCELine = false;
			while(!foundTargetCELine && (targetCELineMapIterator != targetCELineMap.end()))
			{
				//Check if this is the target line
				if(targetCELineMapIterator->second.busLineName == ceLineConditionIterator->first)
				{
					foundTargetCELine = true;
					continue;
				}
				++targetCELineMapIterator;
			}

			//If we found the specified CE line, store details of the CE line condition.
			if(foundTargetCELine)
			{
				//Record the busLineID and value parameters in the mapEntry object, so
				//we can bind them properly later.
				foundTargetCELine = true;
				mapEntry.ceConditions.insert(std::pair<unsigned int, unsigned int>(targetCELineMapIterator->second.busLineID, ceLineConditionIterator->second));
			}

			//Record whether we have matched all CE line conditions examined so far with
			//their line definitions.
			foundAllCELines &= foundTargetCELine;
		}

		//If we couldn't find all the referenced CE lines, return false.
		if(!foundAllCELines)
		{
			return false;
		}
	}

	//Handle an AddressLineMapping parameter
	if(params.addressLineMappingDefined)
	{
		mapEntry.remapAddressLines = true;
		if(!mapEntry.addressLineRemapTable.SetDataMapping(params.addressLineMapping, busMappingAddressBusWidth))
		{
			return false;
		}
	}

	//Handle a DataLineMapping parameter
	if(params.dataLineMappingDefined)
	{
		mapEntry.remapDataLines = true;
		if(!mapEntry.dataLineRemapTable.SetDataMapping(params.dataLineMapping, busMappingDataBusWidth))
		{
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::DoMapEntriesOverlap(const MapEntry& entry1, const MapEntry& entry2) const
{
	//Build a combined address mask. This will ensure we're only considering bits in the
	//address value which are exclusive.
	unsigned int combinedAddressMask = entry2.addressMask & entry1.addressMask;

	//Check that the masked address regions of the two mappings don't overlap. If they do
	//overlap, we need to go further and examine the CE line conditions for the mappings.
	if(((entry1.address & combinedAddressMask) >= ((entry2.address & combinedAddressMask) + entry2.interfaceSize)) //The new map entry starts after the existing map entry
	|| (((entry1.address & combinedAddressMask) + entry1.interfaceSize) <= (entry2.address & combinedAddressMask))) //The new map entry ends before the existing map entry
	{
		//There's no overlap in the masked address region, so the mappings don't overlap.
		//Return false.
		return false;
	}

	//If the addresses overlap, check to see if the CE line conditions also overlap.
	for(std::map<unsigned int, unsigned int>::const_iterator i = entry1.ceConditions.begin(); i != entry1.ceConditions.end(); ++i)
	{
		//Check if the two entries both have a condition on the same CE line
		bool foundMatchingCELine = false;
		std::map<unsigned int, unsigned int>::const_iterator ceConditionsIterator = entry2.ceConditions.begin();
		while(!foundMatchingCELine && (ceConditionsIterator != entry2.ceConditions.end()))
		{
			if(i->first == ceConditionsIterator->first)
			{
				foundMatchingCELine = true;
				continue;
			}
			++ceConditionsIterator;
		}

		//If both entries have a condition on this same CE line, check if the conditions
		//are mutually exclusive. If they don't both have a condition on this line, ignore
		//it, since the condition is not mutually exclusive.
		if(foundMatchingCELine)
		{
			//If the condition values are different, they are mutually exclusive. Return
			//false;
			if(i->second != ceConditionsIterator->second)
			{
				return false;
			}
		}
	}

	//We've found an overlapping entry. Return true.
	return true;
}

//----------------------------------------------------------------------------------------
void BusInterface::AddMapEntryToPhysicalMap(MapEntry* mapEntry, std::vector<ThinList<MapEntry*>*>& physicalMap, unsigned int mappingAddressBusMask) const
{
	//We do a bit of voodoo here to calculate each address where a mask is used. By doing
	//subtraction on the inverted mask, then masking the result with the inverted mask
	//again, we're able to easily calculate a base offset for any mask value. Run some
	//numbers on paper and it should be clear how it works.
	bool done = false;
	unsigned int addValue = ~mapEntry->addressMask & mappingAddressBusMask;
	while(!done)
	{
		unsigned int memoryMapBase = (mapEntry->address + addValue) & mappingAddressBusMask;
		for(unsigned int i = 0; (i < mapEntry->interfaceSize) && ((memoryMapBase + i) <= mappingAddressBusMask); ++i)
		{
			if((memoryMapBase + i) >= physicalMap.size())
			{
				ReleaseAssert(false);
			}
			if(physicalMap[memoryMapBase + i] == 0)
			{
				physicalMap[memoryMapBase + i] = new ThinList<MapEntry*>(mapEntry);
			}
			else
			{
				physicalMap[memoryMapBase + i]->PushToBack(mapEntry);
			}
		}

		if(addValue == 0)
		{
			done = true;
			continue;
		}
		addValue = ((addValue - 1) & ~mapEntry->addressMask) & mappingAddressBusMask;
	}
}

//----------------------------------------------------------------------------------------
void BusInterface::RemoveMapEntryFromPhysicalMap(MapEntry* mapEntry, std::vector<ThinList<MapEntry*>*>& physicalMap, unsigned int mappingAddressBusMask)
{
	//We do a bit of voodoo here to calculate each address where a mask is used. By
	//doing subtraction on the inverted mask, then masking the result with the
	//inverted mask again, we're able to easily calculate a base offset for any mask
	//value. Run some numbers on paper and it should be clear how it works.
	bool done = false;
	unsigned int addValue = ~mapEntry->addressMask & mappingAddressBusMask;
	while(!done)
	{
		unsigned int memoryMapBase = (mapEntry->address + addValue) & mappingAddressBusMask;
		for(unsigned int i = 0; (i < mapEntry->interfaceSize) && ((memoryMapBase + i) <= mappingAddressBusMask); ++i)
		{
			if((memoryMapBase + i) >= physicalMap.size())
			{
				ReleaseAssert(false);
			}

			//Delete the item from the list structure at this location
			ThinList<MapEntry*>* nextMapEntry = physicalMap[memoryMapBase + i];
			if(nextMapEntry->object == mapEntry)
			{
				//If the first entry in the list is the target entry, set the start of the
				//list to be the next list entry, and delete the item list item.
				physicalMap[memoryMapBase + i] = nextMapEntry->next;
				delete nextMapEntry;
			}
			else
			{
				//If the first entry in the list is not the target entry, search the rest
				//of the list looking for a matching item to erase.
				ThinList<MapEntry*>* currentMapEntry = nextMapEntry;
				nextMapEntry = currentMapEntry->next;
				while(nextMapEntry != 0)
				{
					if(nextMapEntry->object == mapEntry)
					{
						//If the next map entry is the target object, remove it from the
						//list.
						currentMapEntry->next = nextMapEntry->next;
						delete nextMapEntry;
					}
					else
					{
						//If the next map entry is not the target object, advance to it,
						//and continue the search.
						currentMapEntry = nextMapEntry;
					}

					//Set the next item in the list as the next map entry to examine
					nextMapEntry = currentMapEntry->next;
				}
			}
		}

		if(addValue == 0)
		{
			done = true;
			continue;
		}
		addValue = ((addValue - 1) & ~mapEntry->addressMask) & mappingAddressBusMask;
	}
}

//----------------------------------------------------------------------------------------
//Memory mapping functions
//----------------------------------------------------------------------------------------
bool BusInterface::MapDevice(IDevice* device, IHeirarchicalStorageNode& node)
{
	//Extract all possible parameters from the XMLEntity object
	DeviceMappingParams params;
	IHeirarchicalStorageAttribute* memoryMapBaseAttribute = node.GetAttribute(L"MemoryMapBase");
	if(memoryMapBaseAttribute != 0)
	{
		params.memoryMapBaseDefined = true;
		params.memoryMapBase = memoryMapBaseAttribute->ExtractHexValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* memoryMapSizeAttribute = node.GetAttribute(L"MemoryMapSize");
	if(memoryMapSizeAttribute != 0)
	{
		params.memoryMapSizeDefined = true;
		params.memoryMapSize = memoryMapSizeAttribute->ExtractHexValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* interfaceOffsetAttribute = node.GetAttribute(L"InterfaceOffset");
	if(interfaceOffsetAttribute != 0)
	{
		params.interfaceOffsetDefined = true;
		params.interfaceOffset = interfaceOffsetAttribute->ExtractHexValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* interfaceNumberAttribute = node.GetAttribute(L"InterfaceNumber");
	if(interfaceNumberAttribute != 0)
	{
		params.interfaceNumberDefined = true;
		params.interfaceNumber = interfaceNumberAttribute->ExtractValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* addressLineFilterAttribute = node.GetAttribute(L"AddressLineFilter");
	if(addressLineFilterAttribute != 0)
	{
		params.addressLineFilterDefined = true;
		params.addressLineFilter = addressLineFilterAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* ceLineConditionsAttribute = node.GetAttribute(L"CELineConditions");
	if(ceLineConditionsAttribute != 0)
	{
		params.ceLineConditionsDefined = true;
		params.ceLineConditions = ceLineConditionsAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* addressLineMappingAttribute = node.GetAttribute(L"AddressLineMapping");
	if(addressLineMappingAttribute != 0)
	{
		params.addressLineMappingDefined = true;
		params.addressLineMapping = addressLineMappingAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* dataLineMappingAttribute = node.GetAttribute(L"DataLineMapping");
	if(dataLineMappingAttribute != 0)
	{
		params.dataLineMappingDefined = true;
		params.dataLineMapping = dataLineMappingAttribute->GetValue();
	}

	//Try and map the device
	return MapDevice(device, params);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapDevice(IDevice* device, const DeviceMappingParams& params)
{
	//Ensure that the properties of the memory interface for this bus have been defined
	if(!memoryInterfaceDefined)
	{
		return false;
	}

	//Create a new MapEntry object
	MapEntry* mapEntry = new MapEntry;
	if(!BuildMapEntry(*mapEntry, device, params, addressBusMask, addressBusWidth, dataBusWidth, true))
	{
		delete mapEntry;
		return false;
	}

	//Add the new MapEntry object to the memory map
	if(!MapDevice(mapEntry))
	{
		delete mapEntry;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapDevice(MapEntry* mapEntry)
{
	//Look for any entries which overlap with the new mapping
	for(unsigned int i = 0; i < (unsigned int)memoryMap.size(); ++i)
	{
		//If we've found an existing mapping which overlaps with the new mapping, return
		//false.
		if(DoMapEntriesOverlap(*memoryMap[i], *mapEntry))
		{
			return false;
		}
	}

	//Add the new entry to the memory map
	memoryMap.push_back(mapEntry);

	//If a physical memory map is being used, add the map entry to the array.
	if(usePhysicalMemoryMap)
	{
		AddMapEntryToPhysicalMap(mapEntry, physicalMemoryMap, addressBusMask);
	}

	return true;
}

//----------------------------------------------------------------------------------------
void BusInterface::UnmapMemoryForDevice(IDevice* device)
{
	//Build a list of all entries which map to the device
	std::list<MapEntry*> entriesToRemove;
	for(unsigned int i = 0; i < (unsigned int)memoryMap.size(); ++i)
	{
		if(memoryMap[i]->device == device)
		{
			entriesToRemove.push_back(memoryMap[i]);
		}
	}

	//Remove each matching entry
	for(std::list<MapEntry*>::const_iterator i = entriesToRemove.begin(); i != entriesToRemove.end(); ++i)
	{
		UnmapDevice(*i);
	}
}

//----------------------------------------------------------------------------------------
void BusInterface::UnmapDevice(MapEntry* mapEntry)
{
	//If a physical memory map is being used, remove the map entry from the array.
	if(usePhysicalMemoryMap)
	{
		RemoveMapEntryFromPhysicalMap(mapEntry, physicalMemoryMap, addressBusMask);
	}

	//Remove the entry from the memory map
	bool done = false;
	std::vector<MapEntry*>::iterator i = memoryMap.begin();
	while(!done && (i != memoryMap.end()))
	{
		if(*i == mapEntry)
		{
			memoryMap.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}
}

//----------------------------------------------------------------------------------------
//Port mapping functions
//----------------------------------------------------------------------------------------
bool BusInterface::MapPort(IDevice* device, IHeirarchicalStorageNode& node)
{
	//Extract all possible parameters from the XMLEntity object
	DeviceMappingParams params;
	IHeirarchicalStorageAttribute* memoryMapBaseAttribute = node.GetAttribute(L"PortMapBase");
	if(memoryMapBaseAttribute != 0)
	{
		params.memoryMapBaseDefined = true;
		params.memoryMapBase = memoryMapBaseAttribute->ExtractHexValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* memoryMapSizeAttribute = node.GetAttribute(L"PortMapSize");
	if(memoryMapSizeAttribute != 0)
	{
		params.memoryMapSizeDefined = true;
		params.memoryMapSize = memoryMapSizeAttribute->ExtractHexValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* interfaceOffsetAttribute = node.GetAttribute(L"InterfaceOffset");
	if(interfaceOffsetAttribute != 0)
	{
		params.interfaceOffsetDefined = true;
		params.interfaceOffset = interfaceOffsetAttribute->ExtractHexValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* interfaceNumberAttribute = node.GetAttribute(L"InterfaceNumber");
	if(interfaceNumberAttribute != 0)
	{
		params.interfaceNumberDefined = true;
		params.interfaceNumber = interfaceNumberAttribute->ExtractValue<unsigned int>();
	}
	IHeirarchicalStorageAttribute* addressLineFilterAttribute = node.GetAttribute(L"AddressLineFilter");
	if(addressLineFilterAttribute != 0)
	{
		params.addressLineFilterDefined = true;
		params.addressLineFilter = addressLineFilterAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* ceLineConditionsAttribute = node.GetAttribute(L"CELineConditions");
	if(ceLineConditionsAttribute != 0)
	{
		params.ceLineConditionsDefined = true;
		params.ceLineConditions = ceLineConditionsAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* addressLineMappingAttribute = node.GetAttribute(L"AddressLineMapping");
	if(addressLineMappingAttribute != 0)
	{
		params.addressLineMappingDefined = true;
		params.addressLineMapping = addressLineMappingAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* dataLineMappingAttribute = node.GetAttribute(L"DataLineMapping");
	if(dataLineMappingAttribute != 0)
	{
		params.dataLineMappingDefined = true;
		params.dataLineMapping = dataLineMappingAttribute->GetValue();
	}

	//Try and map the device
	return MapPort(device, params);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapPort(IDevice* device, const DeviceMappingParams& params)
{
	//Ensure that the properties of the port interface for this bus have been defined
	if(!portInterfaceDefined)
	{
		return false;
	}

	//Create a new MapEntry object
	MapEntry* mapEntry = new MapEntry;
	if(!BuildMapEntry(*mapEntry, device, params, portAddressBusMask, portAddressBusWidth, portDataBusWidth, false))
	{
		delete mapEntry;
		return false;
	}

	//Add the new MapEntry object to the port map
	if(!MapPort(mapEntry))
	{
		delete mapEntry;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapPort(MapEntry* mapEntry)
{
	//Look for any entries which overlap with the new mapping
	for(unsigned int i = 0; i < (unsigned int)portMap.size(); ++i)
	{
		//If we've found an existing mapping which overlaps with the new mapping, return
		//false.
		if(DoMapEntriesOverlap(*portMap[i], *mapEntry))
		{
			return false;
		}
	}

	//Add the new entry to the memory map
	portMap.push_back(mapEntry);

	//If a physical memory map is being used, add the map entry to the array.
	if(usePhysicalPortMap)
	{
		AddMapEntryToPhysicalMap(mapEntry, physicalPortMap, portAddressBusMask);
	}

	return true;
}

//----------------------------------------------------------------------------------------
void BusInterface::UnmapPortForDevice(IDevice* device)
{
	//Build a list of all entries which map to the device
	std::list<MapEntry*> entriesToRemove;
	for(unsigned int i = 0; i < (unsigned int)portMap.size(); ++i)
	{
		if(portMap[i]->device == device)
		{
			entriesToRemove.push_back(portMap[i]);
		}
	}

	//Remove each matching entry
	for(std::list<MapEntry*>::const_iterator i = entriesToRemove.begin(); i != entriesToRemove.end(); ++i)
	{
		UnmapPort(*i);
	}
}

//----------------------------------------------------------------------------------------
void BusInterface::UnmapPort(MapEntry* mapEntry)
{
	//If a physical port map is being used, remove the map entry from the array.
	if(usePhysicalPortMap)
	{
		RemoveMapEntryFromPhysicalMap(mapEntry, physicalPortMap, portAddressBusMask);
	}

	//Remove the entry from the memory map
	bool done = false;
	std::vector<MapEntry*>::iterator i = portMap.begin();
	while(!done && (i != portMap.end()))
	{
		if(*i == mapEntry)
		{
			portMap.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}
}

//----------------------------------------------------------------------------------------
//Line mapping functions
//----------------------------------------------------------------------------------------
bool BusInterface::MapLine(IDevice* sourceDevice, IDevice* targetDevice, IHeirarchicalStorageNode& node)
{
	//Extract all line mapping params from the node
	LineMappingParams params;
	if(!ExtractLineMappingParams(node, params))
	{
		return false;
	}

	//Try and map the line
	return MapLine(sourceDevice, targetDevice, params);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapLine(IDevice* sourceDevice, IDevice* targetDevice, const LineMappingParams& params)
{
	//Abort if we don't have all the required parameters
	if(!params.sourceLineDefined || !params.targetLineDefined)
	{
		return false;
	}

	//If this has been flagged as a mapping template, but it directly targets two devices,
	//it cannot be declared as a template. Return false.
	if(params.declaringPartialMappingTemplate)
	{
		return false;
	}

	//Begin constructing the line mapping
	LineEntry lineEntry;
	lineEntry.sourceDevice = sourceDevice;
	lineEntry.targetDevice = targetDevice;

	//Handle a SourceLine parameter
	std::wstring sourceLineString = params.sourceLine;
	lineEntry.sourceLine = sourceDevice->GetLineID(sourceLineString);
	lineEntry.sourceLineBitCount = sourceDevice->GetLineWidth(lineEntry.sourceLine);
	if(lineEntry.sourceLineBitCount == 0)
	{
		return false;
	}

	//Handle a TargetLine parameter
	std::wstring targetLineString = params.targetLine;
	lineEntry.targetLine = targetDevice->GetLineID(targetLineString);
	lineEntry.targetLineBitCount = targetDevice->GetLineWidth(lineEntry.targetLine);
	if(lineEntry.targetLineBitCount == 0)
	{
		return false;
	}

	//Handle a LineMapping parameter
	if(params.lineMappingDefined)
	{
		std::wstring lineMapping = params.lineMapping;
		lineEntry.remapLines = true;
		if(!lineEntry.lineRemapTable.SetDataMapping(lineMapping, lineEntry.sourceLineBitCount))
		{
			return false;
		}
	}

	//Add the new LineEntry object to the line map
	return MapLine(lineEntry);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapLine(IDevice* sourceDevice, unsigned int targetLineGroupID, IHeirarchicalStorageNode& node)
{
	//Extract all line mapping params from the node
	LineMappingParams params;
	if(!ExtractLineMappingParams(node, params))
	{
		return false;
	}

	//Try and map the line
	return MapLine(sourceDevice, targetLineGroupID, params);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapLine(IDevice* sourceDevice, unsigned int targetLineGroupID, const LineMappingParams& params)
{
	//Abort if we don't have all the required parameters
	if(!params.sourceLineDefined || !params.targetLineDefined)
	{
		return false;
	}

	//If the user is declaring a partial line mapping template, store the template info.
	bool result = true;
	if(params.declaringPartialMappingTemplate)
	{
		//Build the line mapping template
		LineMappingTemplate mappingTemplate;
		mappingTemplate.fromDeviceToGroup = true;
		mappingTemplate.device = sourceDevice;
		mappingTemplate.sourceLine = params.sourceLine;
		mappingTemplate.targetLine = params.targetLine;
		mappingTemplate.lineMappingDefined = params.lineMappingDefined;
		mappingTemplate.lineMapping = params.lineMapping;

		//Add the template to our internal structures
		LineGroupMappingInfo& lineGroupMappingInfo = lineGroupMappingTemplates[targetLineGroupID];
		lineGroupMappingInfo.lineMappingTemplates.push_back(mappingTemplate);
	}
	else
	{
		//Try and find line mapping templates which match the other side of the mapping,
		//and map any we encounter.
		bool foundMatchingTemplate = false;
		bool templateMappingResult = true;
		LineGroupMappingInfo& lineGroupMappingInfo = lineGroupMappingTemplates[targetLineGroupID];
		for(std::list<LineMappingTemplate>::const_iterator i = lineGroupMappingInfo.lineMappingTemplates.begin(); i != lineGroupMappingInfo.lineMappingTemplates.end(); ++i)
		{
			//Check for mirrored mappings which reference a different device
			if((sourceDevice != i->device) && !i->fromDeviceToGroup && (i->sourceLine == params.targetLine))
			{
				foundMatchingTemplate = true;

				//Build the final line mapping params for this line mapping
				LineMappingParams newParams;
				newParams.sourceLineDefined = true;
				newParams.sourceLine = params.sourceLine;
				newParams.targetLineDefined = true;
				newParams.targetLine = i->targetLine;

				//Build the LineMapping parameter
				newParams.lineMappingDefined = false;
				if(params.lineMappingDefined && !i->lineMappingDefined)
				{
					newParams.lineMappingDefined = true;
					newParams.lineMapping = params.lineMapping;
				}
				else if(!params.lineMappingDefined && i->lineMappingDefined)
				{
					newParams.lineMappingDefined = true;
					newParams.lineMapping = i->lineMapping;
				}
				//Ensure that a line mapping has not been defined for both sides of the
				//mapping. If it has, this configuration is not currently supported.
				else if(params.lineMappingDefined && i->lineMappingDefined)
				{
					result = false;
				}

				//Attempt to map the line
				templateMappingResult &= MapLine(sourceDevice, i->device, newParams);
			}
		}

		//Set the result as true if we found at least one matching line mapping template,
		//and all matching line mapping templates were successfully used to create a
		//mapping.
		result = foundMatchingTemplate && templateMappingResult;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapLine(unsigned int sourceLineGroupID, IDevice* targetDevice, IHeirarchicalStorageNode& node)
{
	//Extract all line mapping params from the node
	LineMappingParams params;
	if(!ExtractLineMappingParams(node, params))
	{
		return false;
	}

	//Try and map the line
	return MapLine(sourceLineGroupID, targetDevice, params);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapLine(unsigned int sourceLineGroupID, IDevice* targetDevice, const LineMappingParams& params)
{
	//Abort if we don't have all the required parameters
	if(!params.sourceLineDefined || !params.targetLineDefined)
	{
		return false;
	}

	//If the user is declaring a partial line mapping template, store the template info.
	bool result = true;
	if(params.declaringPartialMappingTemplate)
	{
		//Build the line mapping template
		LineMappingTemplate mappingTemplate;
		mappingTemplate.fromDeviceToGroup = false;
		mappingTemplate.device = targetDevice;
		mappingTemplate.sourceLine = params.sourceLine;
		mappingTemplate.targetLine = params.targetLine;
		mappingTemplate.lineMappingDefined = params.lineMappingDefined;
		mappingTemplate.lineMapping = params.lineMapping;

		//Add the template to our internal structures
		LineGroupMappingInfo& lineGroupMappingInfo = lineGroupMappingTemplates[sourceLineGroupID];
		lineGroupMappingInfo.lineMappingTemplates.push_back(mappingTemplate);
	}
	else
	{
		//Try and find line mapping templates which match the other side of the mapping,
		//and map any we encounter.
		bool foundMatchingTemplate = false;
		bool templateMappingResult = true;
		LineGroupMappingInfo& lineGroupMappingInfo = lineGroupMappingTemplates[sourceLineGroupID];
		for(std::list<LineMappingTemplate>::const_iterator i = lineGroupMappingInfo.lineMappingTemplates.begin(); i != lineGroupMappingInfo.lineMappingTemplates.end(); ++i)
		{
			foundMatchingTemplate = true;

			//Check for mirrored mappings which reference a different device
			if((targetDevice != i->device) && i->fromDeviceToGroup && (i->targetLine == params.sourceLine))
			{
				//Build the final line mapping params for this line mapping
				LineMappingParams newParams;
				newParams.sourceLineDefined = true;
				newParams.sourceLine = i->sourceLine;
				newParams.targetLineDefined = true;
				newParams.targetLine = params.targetLine;

				//Build the LineMapping parameter
				newParams.lineMappingDefined = false;
				if(params.lineMappingDefined && !i->lineMappingDefined)
				{
					newParams.lineMappingDefined = true;
					newParams.lineMapping = params.lineMapping;
				}
				else if(!params.lineMappingDefined && i->lineMappingDefined)
				{
					newParams.lineMappingDefined = true;
					newParams.lineMapping = i->lineMapping;
				}
				//Ensure that a line mapping has not been defined for both sides of the
				//mapping. If it has, this configuration is not currently supported.
				else if(params.lineMappingDefined && i->lineMappingDefined)
				{
					result = false;
				}

				//Attempt to map the line
				templateMappingResult &= MapLine(i->device, targetDevice, newParams);
			}
		}

		//Set the result as true if we found at least one matching line mapping template,
		//and all matching line mapping templates were successfully used to create a
		//mapping.
		result = foundMatchingTemplate && templateMappingResult;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool BusInterface::ExtractLineMappingParams(IHeirarchicalStorageNode& node, LineMappingParams& params) const
{
	//Extract all possible parameters from the XMLEntity object
	IHeirarchicalStorageAttribute* sourceLineAttribute = node.GetAttribute(L"SourceLine");
	if(sourceLineAttribute != 0)
	{
		params.sourceLineDefined = true;
		params.sourceLine = sourceLineAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* targetLineAttribute = node.GetAttribute(L"TargetLine");
	if(targetLineAttribute != 0)
	{
		params.targetLineDefined = true;
		params.targetLine = targetLineAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* lineMappingAttribute = node.GetAttribute(L"LineMapping");
	if(lineMappingAttribute != 0)
	{
		params.lineMappingDefined = true;
		params.lineMapping = lineMappingAttribute->GetValue();
	}
	IHeirarchicalStorageAttribute* declaringPartialMappingTemplate = node.GetAttribute(L"DeclaringPartialMappingTemplate");
	if(declaringPartialMappingTemplate != 0)
	{
		params.declaringPartialMappingTemplate = declaringPartialMappingTemplate->ExtractValue<bool>();
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapLine(const LineEntry& lineEntry)
{
	lineMap.push_back(lineEntry);
	return true;
}

//----------------------------------------------------------------------------------------
void BusInterface::UnmapLineForDevice(IDevice* device)
{
	//Remove any references to the device from the line map
	std::list<LineEntry>::iterator i = lineMap.begin();
	while(i != lineMap.end())
	{
		std::list<LineEntry>::iterator currentEntry = i;
		++i;
		if((currentEntry->sourceDevice == device) || (currentEntry->targetDevice == device))
		{
			lineMap.erase(currentEntry);
		}
	}

	//Remove any line mapping templates which reference the device
	for(LineGroupMappings::iterator lineGroupMappingIterator = lineGroupMappingTemplates.begin(); lineGroupMappingIterator != lineGroupMappingTemplates.end(); ++lineGroupMappingIterator)
	{
		LineGroupMappingInfo& lineGroupMappingInfo = lineGroupMappingIterator->second;
		std::list<LineMappingTemplate>::iterator i = lineGroupMappingInfo.lineMappingTemplates.begin();
		while(i != lineGroupMappingInfo.lineMappingTemplates.end())
		{
			std::list<LineMappingTemplate>::iterator currentEntry = i;
			++i;
			if(currentEntry->device == device)
			{
				lineGroupMappingInfo.lineMappingTemplates.erase(currentEntry);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//CE line mapping functions
//----------------------------------------------------------------------------------------
bool BusInterface::DefineCELineMemory(IHeirarchicalStorageNode& node)
{
	return DefineCELine(node, true);
}

//----------------------------------------------------------------------------------------
bool BusInterface::DefineCELinePort(IHeirarchicalStorageNode& node)
{
	return DefineCELine(node, false);
}

//----------------------------------------------------------------------------------------
bool BusInterface::DefineCELine(IHeirarchicalStorageNode& node, bool memoryMapping)
{
	//Select the target CE line map we want to work with
	CELineMap& targetCELineMap = memoryMapping? ceLineDefinitionsMemory: ceLineDefinitionsPort;

	//Extract all the necessary attributes
	CELineDefinition ceLineDefinition;
	IHeirarchicalStorageAttribute* lineNameAttribute = node.GetAttribute(L"LineName");
	IHeirarchicalStorageAttribute* bitCountAttribute = node.GetAttribute(L"BitCount");
	if((lineNameAttribute == 0) || (bitCountAttribute == 0))
	{
		return false;
	}
	ceLineDefinition.busLineName = lineNameAttribute->GetValue();
	ceLineDefinition.bitCount = bitCountAttribute->ExtractValue<unsigned int>();

	//Extract any optional attributes
	ceLineDefinition.defaultValueDefined = false;
	IHeirarchicalStorageAttribute* defaultValueAttribute = node.GetAttribute(L"DefaultValue");
	if(defaultValueAttribute != 0)
	{
		ceLineDefinition.defaultValueDefined = true;
		ceLineDefinition.defaultValue = defaultValueAttribute->ExtractHexValue<unsigned int>();
	}

	//Ensure a CE line with the same name hasn't already been defined
	for(CELineMap::const_iterator i = targetCELineMap.begin(); i != targetCELineMap.end(); ++i)
	{
		if(i->second.busLineName == ceLineDefinition.busLineName)
		{
			return false;
		}
	}

	//Generate a unique ID for the CE line
	ceLineDefinition.busLineID = nextCELineID++;
	while(targetCELineMap.find(ceLineDefinition.busLineID) != targetCELineMap.end())
	{
		ceLineDefinition.busLineID = nextCELineID++;
	}

	//Insert the new CE line definition into our map
	targetCELineMap.insert(CELineMapEntry(ceLineDefinition.busLineID, ceLineDefinition));

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapCELineInputMemory(IDevice* device, IHeirarchicalStorageNode& node)
{
	return MapCELineInput(device, node, true);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapCELineInputPort(IDevice* device, IHeirarchicalStorageNode& node)
{
	return MapCELineInput(device, node, false);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapCELineInput(IDevice* device, IHeirarchicalStorageNode& node, bool memoryMapping)
{
	//Select the target CE line maps we want to work with
	CELineMap& targetCELineMap = ceLineDefinitionsMemory;
	std::vector<CELineDeviceEntry>& targetCELineDeviceMappings = ceLineDeviceMappingsMemory;

	//Attempt to locate an existing mapping entry for this device
	bool foundTargetDeviceMapping = false;
	unsigned int deviceMappingIndex = 0;
	while(!foundTargetDeviceMapping && (deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size()))
	{
		if(targetCELineDeviceMappings[deviceMappingIndex].device == device)
		{
			foundTargetDeviceMapping = true;
			continue;
		}
		++deviceMappingIndex;
	}

	//If no existing mapping was found, create a new mapping object.
	if(!foundTargetDeviceMapping)
	{
		CELineDeviceEntry deviceEntry;
		deviceEntry.device = device;
		deviceMappingIndex = (unsigned int)targetCELineDeviceMappings.size();
		targetCELineDeviceMappings.push_back(deviceEntry);
	}

	//Obtain a reference to the mapping entry
	CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];

	//Extract all the necessary attributes
	IHeirarchicalStorageAttribute* busLineNameAttribute = node.GetAttribute(L"BusLineName");
	IHeirarchicalStorageAttribute* deviceLineNameAttribute = node.GetAttribute(L"DeviceLineName");
	if((busLineNameAttribute == 0) || (deviceLineNameAttribute == 0))
	{
		return false;
	}
	std::wstring busLineName = busLineNameAttribute->GetValue();
	std::wstring deviceLineName = deviceLineNameAttribute->GetValue();

	//Ensure that this line hasn't already been declared as an input for the device
	for(std::list<CELineDeviceLineInput>::const_iterator i = deviceEntry.lineInputs.begin(); i != deviceEntry.lineInputs.end(); ++i)
	{
		//If this existing line output mapping has the same device line name as the new
		//mapping, return false.
		if(i->deviceLineName == deviceLineName)
		{
			return false;
		}
	}

	//Obtain the bus line ID number for the specified line
	unsigned int busLineID;
	bool foundBusLineID = false;
	CELineMap::const_iterator lineMapIterator = targetCELineMap.begin();
	while(!foundBusLineID && (lineMapIterator != targetCELineMap.end()))
	{
		if(lineMapIterator->second.busLineName == busLineName)
		{
			foundBusLineID = true;
			busLineID = lineMapIterator->second.busLineID;
			continue;
		}
		++lineMapIterator;
	}

	//If we couldn't locate a defined bus line with the specified name, return false.
	if(!foundBusLineID)
	{
		return false;
	}

	//Obtain the device line ID number for the device CE line with the specified name
	unsigned int deviceLineID = device->GetCELineID(deviceLineName, true);
	if(deviceLineID <= 0)
	{
		return false;
	}

	//Build the line input mapping, and add it to the device mapping.
	CELineDeviceLineInput lineMapping;
	lineMapping.deviceLineName = deviceLineName;
	lineMapping.deviceLineID = deviceLineID;
	lineMapping.busLineID = busLineID;
	lineMapping.bitNumberAllocated = false;
	deviceEntry.lineInputs.push_back(lineMapping);

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapCELineOutputMemory(IDevice* device, IHeirarchicalStorageNode& node)
{
	return MapCELineOutput(device, node, true);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapCELineOutputPort(IDevice* device, IHeirarchicalStorageNode& node)
{
	return MapCELineOutput(device, node, false);
}

//----------------------------------------------------------------------------------------
bool BusInterface::MapCELineOutput(IDevice* device, IHeirarchicalStorageNode& node, bool memoryMapping)
{
	//Select the target CE line maps we want to work with
	CELineMap& targetCELineMap = ceLineDefinitionsMemory;
	std::vector<CELineDeviceEntry>& targetCELineDeviceMappings = ceLineDeviceMappingsMemory;

	//Attempt to locate an existing mapping entry for this device
	bool foundTargetDeviceMapping = false;
	unsigned int deviceMappingIndex = 0;
	while(!foundTargetDeviceMapping && (deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size()))
	{
		if(targetCELineDeviceMappings[deviceMappingIndex].device == device)
		{
			foundTargetDeviceMapping = true;
			continue;
		}
		++deviceMappingIndex;
	}

	//If no existing mapping was found, create a new mapping object.
	if(!foundTargetDeviceMapping)
	{
		CELineDeviceEntry deviceEntry;
		deviceEntry.device = device;
		deviceMappingIndex = (unsigned int)targetCELineDeviceMappings.size();
		targetCELineDeviceMappings.push_back(deviceEntry);
	}

	//Obtain a reference to the mapping entry
	CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];

	//Extract all the necessary attributes
	IHeirarchicalStorageAttribute* busLineNameAttribute = node.GetAttribute(L"BusLineName");
	IHeirarchicalStorageAttribute* deviceLineNameAttribute = node.GetAttribute(L"DeviceLineName");
	if((busLineNameAttribute == 0) || (deviceLineNameAttribute == 0))
	{
		return false;
	}
	std::wstring busLineName = busLineNameAttribute->GetValue();
	std::wstring deviceLineName = deviceLineNameAttribute->GetValue();

	//Ensure that this line hasn't already been declared as an output for the device
	for(std::list<CELineDeviceLineOutput>::const_iterator i = deviceEntry.lineOutputs.begin(); i != deviceEntry.lineOutputs.end(); ++i)
	{
		//If this existing line output mapping has the same device line name as the new
		//mapping, return false.
		if(i->deviceLineName == deviceLineName)
		{
			return false;
		}
	}

	//Obtain the bus line ID number for the specified line
	unsigned int busLineID;
	bool foundBusLineID = false;
	CELineMap::const_iterator lineMapIterator = targetCELineMap.begin();
	while(!foundBusLineID && (lineMapIterator != targetCELineMap.end()))
	{
		if(lineMapIterator->second.busLineName == busLineName)
		{
			foundBusLineID = true;
			busLineID = lineMapIterator->second.busLineID;
			continue;
		}
		++lineMapIterator;
	}

	//If we couldn't locate a defined bus line with the specified name, return false.
	if(!foundBusLineID)
	{
		return false;
	}

	//Obtain the device line ID number for the device CE line with the specified name
	unsigned int deviceLineID = device->GetCELineID(deviceLineName, false);
	if(deviceLineID <= 0)
	{
		return false;
	}

	//Build the line output mapping, and add it to the device mapping.
	CELineDeviceLineOutput lineMapping;
	lineMapping.deviceLineName = deviceLineName;
	lineMapping.deviceLineID = deviceLineID;
	lineMapping.busLineID = busLineID;
	lineMapping.bitNumberAllocated = false;
	deviceEntry.lineOutputs.push_back(lineMapping);

	return true;
}

//----------------------------------------------------------------------------------------
bool BusInterface::BindCELineMappings()
{
	//Perform the operation for both memory and port mappings, and return the combined
	//result.
	bool result = true;
	result &= BindCELineMappings(true);
	result &= BindCELineMappings(false);
	return result;
}

//----------------------------------------------------------------------------------------
bool BusInterface::BindCELineMappings(bool memoryMapping)
{
	//Select the target CE line maps we want to work with
	CELineMap& targetCELineMap = memoryMapping? ceLineDefinitionsMemory: ceLineDefinitionsPort;
	unsigned int& targetCELineDeviceMappingsOutputDeviceSize = memoryMapping? ceLineDeviceMappingsMemoryOutputDeviceSize: ceLineDeviceMappingsPortOutputDeviceSize;
	unsigned int& targetCELineInitialState = memoryMapping? ceLineInitialStateMemory: ceLineInitialStatePort;
	std::vector<CELineDeviceEntry>& targetCELineDeviceMappings = memoryMapping? ceLineDeviceMappingsMemory: ceLineDeviceMappingsPort;
	std::vector<MapEntry*>& targetMap = memoryMapping? memoryMap: portMap;

	//For each CE line, build lists of all devices which export and import those lines. We
	//also clear all existing line bit number allocations here.
	std::map<unsigned int, std::list<unsigned int>> deviceOutputLineMap;
	std::map<unsigned int, std::list<unsigned int>> deviceInputLineMap;
	for(unsigned int deviceMappingIndex = 0; deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size(); ++deviceMappingIndex)
	{
		CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];
		for(std::list<CELineDeviceLineOutput>::iterator i = deviceEntry.lineOutputs.begin(); i != deviceEntry.lineOutputs.end(); ++i)
		{
			deviceOutputLineMap[i->busLineID].push_back(deviceMappingIndex);
			i->bitNumberAllocated = false;
		}
		for(std::list<CELineDeviceLineInput>::iterator i = deviceEntry.lineInputs.begin(); i != deviceEntry.lineInputs.end(); ++i)
		{
			deviceInputLineMap[i->busLineID].push_back(deviceMappingIndex);
			i->bitNumberAllocated = false;
		}
	}

	//Build a set of all CE lines which are used in mappings
	std::set<unsigned int> ceLinesUsedDirectlyInMappings;
	for(std::vector<MapEntry*>::iterator mapEntryIterator = targetMap.begin(); mapEntryIterator != targetMap.end(); ++mapEntryIterator)
	{
		//Iterate over all the CE line conditions for this map entry, and add each
		//referenced CE line to our set of used CE lines.
		MapEntry& mapEntry = *(*mapEntryIterator);
		for(std::map<unsigned int, unsigned int>::const_iterator i = mapEntry.ceConditions.begin(); i != mapEntry.ceConditions.end(); ++i)
		{
			ceLinesUsedDirectlyInMappings.insert(i->first);
		}
	}

	//Build a set of all devices that export CE lines, where at least one of the exported
	//CE lines is used in an actual mapping, either directly, or indirectly as a CE line
	//input for another device that has a used line.
	std::set<unsigned int> usedDeviceSet;
	bool finishedBuildingUsedDeviceSet = false;
	while(!finishedBuildingUsedDeviceSet)
	{
		unsigned int devicesAddedThisIteration = 0;
		for(unsigned int deviceMappingIndex = 0; deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size(); ++deviceMappingIndex)
		{
			//If we've already added this device mapping to the set of used devices, skip
			//it.
			if(usedDeviceSet.find(deviceMappingIndex) != usedDeviceSet.end())
			{
				continue;
			}

			//Iterate through all the output lines for this device until we find one which
			//is used for a mapping, or we've examined all the output lines.
			bool foundUsedOutputLine = false;
			CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];
			std::list<CELineDeviceLineOutput>::const_iterator lineOutputIterator = deviceEntry.lineOutputs.begin();
			while(!foundUsedOutputLine && (lineOutputIterator != deviceEntry.lineOutputs.end()))
			{
				//Check if this line is used directly in any mappings
				if(ceLinesUsedDirectlyInMappings.find(lineOutputIterator->busLineID) != ceLinesUsedDirectlyInMappings.end())
				{
					foundUsedOutputLine = true;
					continue;
				}

				//Iterate through the entries for this line in deviceInputLineMap. If
				//another device uses this CE line as an input for calculating its own CE
				//lines, and that device has been added to the used device set, flag that
				//we've found a used output line.
				std::map<unsigned int, std::list<unsigned int>>::const_iterator deviceInputLineMapIterator = deviceInputLineMap.find(lineOutputIterator->busLineID);
				if(deviceInputLineMapIterator != deviceInputLineMap.end())
				{
					//Iterate over the list of devices which import this CE line, and flag
					//the output line as used if we find a used device which imports this
					//line.
					const std::list<unsigned int>& deviceIDList = deviceInputLineMapIterator->second;
					std::list<unsigned int>::const_iterator deviceIDListIterator = deviceIDList.begin();
					while(!foundUsedOutputLine && (deviceIDListIterator != deviceIDList.end()))
					{
						//If the device which imports this CE line has been found to be
						//used in a mapping, flag that we've found a used output line for
						//our device.
						if(usedDeviceSet.find(*deviceIDListIterator) != usedDeviceSet.end())
						{
							foundUsedOutputLine = true;
							continue;
						}
						++deviceIDListIterator;
					}
				}

				//Advance to the next line output entry
				++lineOutputIterator;
			}

			//If we found an output line that was used, add this device to the used device
			//set.
			if(foundUsedOutputLine)
			{
				usedDeviceSet.insert(deviceMappingIndex);
				++devicesAddedThisIteration;
			}
		}

		//If no more devices were added to the used device set in the last iteration, all
		//used devices have now been found. Flag the operation as complete.
		if(devicesAddedThisIteration == 0)
		{
			finishedBuildingUsedDeviceSet = true;
		}
	}

	//Build a set of all CE lines which are used in an actual mapping, either directly, or
	//indirectly as a CE line input for another device that has a used line.
	std::set<unsigned int> usedCELineSet;
	for(unsigned int deviceMappingIndex = 0; deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size(); ++deviceMappingIndex)
	{
		CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];
		for(std::list<CELineDeviceLineOutput>::const_iterator lineOutputIterator = deviceEntry.lineOutputs.begin(); lineOutputIterator != deviceEntry.lineOutputs.end(); ++lineOutputIterator)
		{
			//If this line is used directly in any mappings, add it to the used CE line
			//set.
			if(ceLinesUsedDirectlyInMappings.find(lineOutputIterator->busLineID) != ceLinesUsedDirectlyInMappings.end())
			{
				usedCELineSet.insert(lineOutputIterator->busLineID);
				continue;
			}

			//Iterate through the entries for this line in deviceInputLineMap. If another
			//device uses this CE line as an input for calculating its own CE lines, and
			//that device has been determined to have an output CE line that is used by a
			//mapping, either directly or indirectly, add this line to the used CE line
			//set.
			std::map<unsigned int, std::list<unsigned int>>::const_iterator deviceInputLineMapIterator = deviceInputLineMap.find(lineOutputIterator->busLineID);
			if(deviceInputLineMapIterator != deviceInputLineMap.end())
			{
				//Iterate over the list of devices which import this CE line, and flag
				//the output line as used if we find a used device which imports this
				//line.
				bool foundUseOfLine = false;
				const std::list<unsigned int>& deviceIDList = deviceInputLineMapIterator->second;
				std::list<unsigned int>::const_iterator deviceIDListIterator = deviceIDList.begin();
				while(!foundUseOfLine && (deviceIDListIterator != deviceIDList.end()))
				{
					//If the device which imports this CE line has been found to be
					//used in a mapping, flag that we've found a used output line for
					//our device.
					if(usedDeviceSet.find(*deviceIDListIterator) != usedDeviceSet.end())
					{
						foundUseOfLine = true;
						continue;
					}
					++deviceIDListIterator;
				}

				//If we found a use device which imports this CE line, add it to the used
				//CE line set.
				if(foundUseOfLine)
				{
					usedCELineSet.insert(lineOutputIterator->busLineID);
				}
			}
		}
	}

	//For each device which exports CE lines, and where at least one of those CE lines is
	//used by an actual mapping, build a list of all devices it is directly dependent on,
	//IE, all devices which export a CE line that it imports.
	std::map<unsigned int, std::set<unsigned int>> deviceDependencyMap;
	bool undefinedInputCELine = false;
	for(unsigned int deviceMappingIndex = 0; deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size(); ++deviceMappingIndex)
	{
		//If this device doesn't export any CE lines that are used, either directly or
		//indirectly, in any actual mappings, skip it.
		if(usedDeviceSet.find(deviceMappingIndex) == usedDeviceSet.end())
		{
			continue;
		}

		//Ensure that this device is added to the device dependency map
		deviceDependencyMap[deviceMappingIndex].clear();

		//Iterate through the list of all imported lines for this device, and add a
		//dependency on all devices which export those lines.
		CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];
		for(std::list<CELineDeviceLineInput>::const_iterator lineInputIterator = deviceEntry.lineInputs.begin(); lineInputIterator != deviceEntry.lineInputs.end(); ++lineInputIterator)
		{
			//Try and locate the list of devices which output this line
			std::map<unsigned int, std::list<unsigned int>>::const_iterator exportingDevicesForLineIterator = deviceOutputLineMap.find(lineInputIterator->busLineID);
			if(exportingDevicesForLineIterator != deviceOutputLineMap.end())
			{
				//Add the ID numbers of all devices which export this line to the list of
				//dependencies for this device.
				for(std::list<unsigned int>::const_iterator i = exportingDevicesForLineIterator->second.begin(); i != exportingDevicesForLineIterator->second.end(); ++i)
				{
					//Only add the dependency if the device exporting the line is
					//different from the device we're evaluating dependencies for. It is
					//valid for a device to both import and export the same CE line, in
					//which case it is passed as an input, but may also be modified by
					//that device.
					if(*i != deviceMappingIndex)
					{
						deviceDependencyMap[deviceMappingIndex].insert(*i);
					}
				}
			}
			else
			{
				//If we can't find an entry for this line, it means that no devices output
				//it. If this is the case, a default value for the CE line must be
				//defined. Check the definition of the CE line to ensure a default value
				//is specified.
				CELineMap::const_iterator targetCELineIterator = targetCELineMap.find(lineInputIterator->busLineID);
				if((targetCELineIterator == targetCELineMap.end()) || !targetCELineIterator->second.defaultValueDefined)
				{
					//If no default value has been specified for this CE line, we have an
					//undefined input. Flag the error.
					undefinedInputCELine = true;
				}
			}
		}
	}

	//If there are undefined input CE lines for any devices, return false.
	if(undefinedInputCELine)
	{
		return false;
	}

	//Build a sorted list indicating the order to call each device in when building the CE
	//line state, which satisfies all line input and output requirements. Only devices
	//which export CE lines that are used in our bus mappings are examined.
	std::vector<CELineDeviceEntry> targetCELineDeviceMappingsSorted;
	targetCELineDeviceMappingsSorted.reserve(targetCELineDeviceMappings.size());
	std::set<unsigned int> addedDeviceSet;
	bool deviceSortSucceeded = false;
	bool finishedSortingDevices = false;
	while(!finishedSortingDevices)
	{
		//Iterate over all the remaining devices, looking for devices that can be added to
		//our sorted list.
		unsigned int devicesAddedThisIteration = 0;
		std::map<unsigned int, std::set<unsigned int>>::iterator nextDeviceIterator = deviceDependencyMap.begin();
		while(nextDeviceIterator != deviceDependencyMap.end())
		{
			//Check if all devices we're dependent on have been added
			bool allDependenciesAdded = true;
			for(std::set<unsigned int>::const_iterator i = nextDeviceIterator->second.begin(); i != nextDeviceIterator->second.end(); ++i)
			{
				allDependenciesAdded &= (addedDeviceSet.find(*i) != addedDeviceSet.end());
			}

			//Determine whether to skip this device, or add it to the sorted list.
			if(allDependenciesAdded)
			{
				//If all dependencies for this device have been added, add it to our sorted
				//array, and remove it from the list of devices to sort.
				std::map<unsigned int, std::set<unsigned int>>::iterator eraseIterator = nextDeviceIterator;
				addedDeviceSet.insert(nextDeviceIterator->first);
				targetCELineDeviceMappingsSorted.push_back(targetCELineDeviceMappings[nextDeviceIterator->first]);
				++nextDeviceIterator;
				++devicesAddedThisIteration;
				deviceDependencyMap.erase(eraseIterator);
			}
			else
			{
				//If one or more devices we're dependent on haven't been added to the
				//sorted list yet, skip this device until all its dependencies have been
				//added.
				++nextDeviceIterator;
			}
		}

		//Check if we've finished sorting the devices
		if(deviceDependencyMap.empty())
		{
			//If all devices have been added to the sorted list, we're done.
			deviceSortSucceeded = true;
			finishedSortingDevices = true;
		}
		else if(devicesAddedThisIteration == 0)
		{
			//If no devices were successfully added in the last pass, we've hit a circular
			//dependency. Break out of the device loop, and flag that the operation
			//failed.
			deviceSortSucceeded = false;
			finishedSortingDevices = true;
		}
	}

	//If we failed to determine an order for the devices, return false.
	if(!deviceSortSucceeded)
	{
		return false;
	}

	//Now that we've added all the "active" devices to the list, sorted in the correct
	//order to satisfy dependencies between them, we need to record the number of entries
	//we have added to the list so far. Only these devices will be called to evalulate the
	//CE line state on bus access.
	targetCELineDeviceMappingsOutputDeviceSize = (unsigned int)targetCELineDeviceMappings.size();

	//Add all remaining device mappings into the sorted array. These remaining mappings
	//won't be used to calculate the CE line state, but need to be kept for reference, in
	//particular in case a new device is mapped in later on, and a CE line which was
	//previously unused is now being used.
	for(unsigned int deviceMappingIndex = 0; deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size(); ++deviceMappingIndex)
	{
		if(addedDeviceSet.find(deviceMappingIndex) == addedDeviceSet.end())
		{
			targetCELineDeviceMappingsSorted.push_back(targetCELineDeviceMappings[deviceMappingIndex]);
		}
	}

	//Now that the sort is complete, replace the original unsorted array with our sorted
	//version.
	targetCELineDeviceMappings = targetCELineDeviceMappingsSorted;

	//Build the line masks for each device, and bit numbers and masks for each line in
	//each device.
	//##TODO## Find a way to refactor this, to avoid the duplicated code below.
	unsigned int nextBitNumber = 0;
	std::map<unsigned int, unsigned int> bitNumbersAllocated;
	for(unsigned int deviceMappingIndex = 0; deviceMappingIndex < targetCELineDeviceMappingsOutputDeviceSize; ++deviceMappingIndex)
	{
		CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];

		//Allocate bit numbers for each input line
		unsigned int inputLineMask = 0;
		for(std::list<CELineDeviceLineInput>::iterator i = deviceEntry.lineInputs.begin(); i != deviceEntry.lineInputs.end(); ++i)
		{
			//Attempt to retrieve the number of bits in this CE line
			unsigned int lineBitCount;
			CELineMap::const_iterator targetCELineDefinitionIterator = targetCELineMap.find(i->busLineID);
			if(targetCELineDefinitionIterator == targetCELineMap.end())
			{
				return false;
			}
			lineBitCount = targetCELineDefinitionIterator->second.bitCount;

			//Determine the bit number to allocate to this line
			unsigned int lineBitNumber;
			std::map<unsigned int, unsigned int>::const_iterator bitNumberForLineIterator = bitNumbersAllocated.find(i->busLineID);
			if(bitNumberForLineIterator != bitNumbersAllocated.end())
			{
				//If this line has already been allocated a bit number, use the stored
				//number.
				lineBitNumber = bitNumberForLineIterator->second;
			}
			else
			{
				//If this line hasn't been allocated a bit number, allocate the next
				//available bit, and record the allocation.
				lineBitNumber = nextBitNumber;
				bitNumbersAllocated.insert(std::pair<unsigned int, unsigned int>(i->busLineID, lineBitNumber));

				//Set the next available bit number to the next bit after this line
				nextBitNumber += lineBitCount;
			}

			//Store the allocated bit number for this line in the mapping
			i->bitNumberAllocated = true;
			i->lineBitNumber = lineBitNumber;

			//Build the bitmask for this line. Note that the unusual mask generation is to
			//support a value for lineBitCount which is equal to the bitcount of the
			//underlying storage size of lineBitmask. Performing a shift equal to the size
			//of a datatype is undefined in C++.
			i->lineBitmask = ((((1 << (lineBitCount - 1)) - 1) << 1) | 0x01) << i->lineBitNumber;

			//Combine the bitmask for this line into the overall mask for the lines
			inputLineMask |= i->lineBitmask;
		}

		//Allocate bit numbers for each output line
		unsigned int outputLineMask = 0;
		for(std::list<CELineDeviceLineOutput>::iterator i = deviceEntry.lineOutputs.begin(); i != deviceEntry.lineOutputs.end(); ++i)
		{
			//Ensure this output line is actually used by a mapping, or as an input line
			//for another device we need to calculate CE output lines for.
			if(usedCELineSet.find(i->busLineID) == usedCELineSet.end())
			{
				continue;
			}

			//Attempt to retrieve the number of bits in this CE line
			unsigned int lineBitCount;
			CELineMap::const_iterator targetCELineDefinitionIterator = targetCELineMap.find(i->busLineID);
			if(targetCELineDefinitionIterator == targetCELineMap.end())
			{
				return false;
			}
			lineBitCount = targetCELineDefinitionIterator->second.bitCount;

			//Determine the bit number to allocate to this line
			unsigned int lineBitNumber;
			std::map<unsigned int, unsigned int>::const_iterator bitNumberForLineIterator = bitNumbersAllocated.find(i->busLineID);
			if(bitNumberForLineIterator != bitNumbersAllocated.end())
			{
				//If this line has already been allocated a bit number, use the stored
				//number.
				lineBitNumber = bitNumberForLineIterator->second;
			}
			else
			{
				//If this line hasn't been allocated a bit number, allocate the next
				//available bit, and record the allocation.
				lineBitNumber = nextBitNumber;
				bitNumbersAllocated.insert(std::pair<unsigned int, unsigned int>(i->busLineID, lineBitNumber));

				//Set the next available bit number to the next bit after this line
				nextBitNumber += lineBitCount;
			}

			//Store the allocated bit number for this line in the mapping
			i->bitNumberAllocated = true;
			i->lineBitNumber = lineBitNumber;

			//Build the bitmask for this line. Note that the unusual mask generation is to
			//support a value for lineBitCount which is equal to the bitcount of the
			//underlying storage size of lineBitmask. Performing a shift equal to the size
			//of a datatype is undefined in C++.
			i->lineBitmask = ((((1 << (lineBitCount - 1)) - 1) << 1) | 0x01) << i->lineBitNumber;

			//Combine the bitmask for this line into the overall mask for the lines
			outputLineMask |= i->lineBitmask;
		}

		//Store the calculated line masks
		deviceEntry.inputCELineMask = inputLineMask;
		deviceEntry.outputCELineMask = outputLineMask;
	}

	//Now that bit numbers have been allocated for our CE lines, build the CE line
	//condition values and masks in each of the mapping entries.
	for(std::vector<MapEntry*>::iterator mapEntryIterator = targetMap.begin(); mapEntryIterator != targetMap.end(); ++mapEntryIterator)
	{
		//Iterate over all the CE line conditions for this map entry, and build a CE line
		//condition value and mask to use during address decoding.
		MapEntry& mapEntry = *(*mapEntryIterator);
		mapEntry.ce = 0;
		mapEntry.ceMask = 0;
		for(std::map<unsigned int, unsigned int>::const_iterator i = mapEntry.ceConditions.begin(); i != mapEntry.ceConditions.end(); ++i)
		{
			//Obtain a reference to the definition of the target CE line
			CELineMap::const_iterator targetCELineDefinitionIterator = targetCELineMap.find(i->first);
			if(targetCELineDefinitionIterator == targetCELineMap.end())
			{
				return false;
			}
			const CELineDefinition& lineDefinition = targetCELineDefinitionIterator->second;

			//If the specified CE line has not been allocated a bit number yet, it means
			//that no device currently outputs that line. In this case, we ensure the
			//target CE line has a default value assigned, and allocate a bit number for
			//it here.
			unsigned int lineBitNumber;
			std::map<unsigned int, unsigned int>::const_iterator bitNumberForLineIterator = bitNumbersAllocated.find(i->first);
			if(bitNumberForLineIterator != bitNumbersAllocated.end())
			{
				//If this line has already been allocated a bit number, use the stored
				//number.
				lineBitNumber = bitNumberForLineIterator->second;
			}
			else
			{
				//Ensure a default value has been specified for this line
				if(!lineDefinition.defaultValueDefined)
				{
					return false;
				}

				//If this line hasn't been allocated a bit number, allocate the next
				//available bit, and record the allocation.
				lineBitNumber = nextBitNumber++;
				bitNumbersAllocated.insert(std::pair<unsigned int, unsigned int>(i->first, lineBitNumber));
			}

			//Calculate the bitmask for the target line
			unsigned int lineBitmask = ((((1 << (lineDefinition.bitCount - 1)) - 1) << 1) | 0x01) << lineBitNumber;

			//Combine the condition value for the target CE line into the overall CE line
			//condition value for this entry
			mapEntry.ce |= (i->second << lineBitNumber) & lineBitmask;
			mapEntry.ceMask |= lineBitmask;
		}
	}

	//Build the initial CE line state value
	targetCELineInitialState = 0;
	for(CELineMap::const_iterator i = targetCELineMap.begin(); i != targetCELineMap.end(); ++i)
	{
		//If this line has a default value specified, retrieve its bit number, and combine
		//its default value into the initial CE line state value.
		if(i->second.defaultValueDefined)
		{
			//Try and retrieve the bit number for this line, and store the default value
			//for the line at the specified bit location. The only reason a bit number
			//would not be allocated is if no device is using the line, in which case, we
			//don't need to store a default value.
			std::map<unsigned int, unsigned int>::const_iterator bitNumberForLineIterator = bitNumbersAllocated.find(i->second.busLineID);
			if(bitNumberForLineIterator != bitNumbersAllocated.end())
			{
				unsigned int lineBitNumber = bitNumberForLineIterator->second;
				unsigned int lineBitmask = ((((1 << (i->second.bitCount - 1)) - 1) << 1) | 0x01);
				targetCELineInitialState |= (i->second.defaultValue & lineBitmask) << lineBitNumber;
			}
		}
	}

	//Pass the bit numbers for each line to each device
	for(unsigned int deviceMappingIndex = 0; deviceMappingIndex < (unsigned int)targetCELineDeviceMappings.size(); ++deviceMappingIndex)
	{
		CELineDeviceEntry& deviceEntry = targetCELineDeviceMappings[deviceMappingIndex];
		for(std::list<CELineDeviceLineInput>::iterator i = deviceEntry.lineInputs.begin(); i != deviceEntry.lineInputs.end(); ++i)
		{
			deviceEntry.device->SetCELineInput(i->deviceLineID, i->bitNumberAllocated, i->lineBitNumber);
		}
		for(std::list<CELineDeviceLineOutput>::iterator i = deviceEntry.lineOutputs.begin(); i != deviceEntry.lineOutputs.end(); ++i)
		{
			deviceEntry.device->SetCELineOutput(i->deviceLineID, i->bitNumberAllocated, i->lineBitNumber);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void BusInterface::UnmapCELinesForDevice(IDevice* device)
{
	//Delete memory CE lines defined by this device
	std::list<unsigned int> memoryMappingsToDelete;
	for(unsigned int i = 0; i < (unsigned int)ceLineDeviceMappingsMemory.size(); ++i)
	{
		if(ceLineDeviceMappingsMemory[i].device == device)
		{
			memoryMappingsToDelete.push_back(i);
		}
	}
	for(std::list<unsigned int>::const_reverse_iterator i = memoryMappingsToDelete.rbegin(); i != memoryMappingsToDelete.rend(); ++i)
	{
		ceLineDeviceMappingsMemory.erase(ceLineDeviceMappingsMemory.begin() + *i);
	}

	//Delete port CE lines defined by this device
	std::list<unsigned int> portMappingsToDelete;
	for(unsigned int i = 0; i < (unsigned int)ceLineDeviceMappingsPort.size(); ++i)
	{
		if(ceLineDeviceMappingsPort[i].device == device)
		{
			portMappingsToDelete.push_back(i);
		}
	}
	for(std::list<unsigned int>::const_reverse_iterator i = portMappingsToDelete.rbegin(); i != portMappingsToDelete.rend(); ++i)
	{
		ceLineDeviceMappingsPort.erase(ceLineDeviceMappingsPort.begin() + *i);
	}
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int BusInterface::CalculateCELineStateMemory(unsigned int location, const Data& data, IDeviceContext* caller, double accessTime) const
{
	unsigned int ceLineState = ceLineInitialStateMemory;

	//Combine the CE line state output for each device
	for(unsigned int i = 0; i < ceLineDeviceMappingsMemoryOutputDeviceSize; ++i)
	{
		const CELineDeviceEntry& deviceEntry = ceLineDeviceMappingsMemory[i];
		unsigned int deviceReturn = deviceEntry.device->CalculateCELineStateMemory(location, data, ceLineState & deviceEntry.inputCELineMask, this, caller, accessTime);
		ceLineState |= (deviceReturn & deviceEntry.outputCELineMask);
	}

	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int BusInterface::CalculateCELineStateMemoryTransparent(unsigned int location, const Data& data, IDeviceContext* caller) const
{
	unsigned int ceLineState = ceLineInitialStateMemory;

	//Combine the CE line state output for each device
	for(unsigned int i = 0; i < ceLineDeviceMappingsMemoryOutputDeviceSize; ++i)
	{
		const CELineDeviceEntry& deviceEntry = ceLineDeviceMappingsMemory[i];
		unsigned int deviceReturn = deviceEntry.device->CalculateCELineStateMemoryTransparent(location, data, ceLineState & deviceEntry.inputCELineMask, this, caller);
		ceLineState |= (deviceReturn & deviceEntry.outputCELineMask);
	}

	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int BusInterface::CalculateCELineStatePort(unsigned int location, const Data& data, IDeviceContext* caller, double accessTime) const
{
	unsigned int ceLineState = ceLineInitialStatePort;

	//Combine the CE line state output for each device
	for(unsigned int i = 0; i < ceLineDeviceMappingsPortOutputDeviceSize; ++i)
	{
		const CELineDeviceEntry& deviceEntry = ceLineDeviceMappingsPort[i];
		unsigned int deviceReturn = deviceEntry.device->CalculateCELineStatePort(location, data, ceLineState & deviceEntry.inputCELineMask, this, caller, accessTime);
		ceLineState |= (deviceReturn & deviceEntry.outputCELineMask);
	}

	return ceLineState;
}

//----------------------------------------------------------------------------------------
unsigned int BusInterface::CalculateCELineStatePortTransparent(unsigned int location, const Data& data, IDeviceContext* caller) const
{
	unsigned int ceLineState = ceLineInitialStatePort;

	//Combine the CE line state output for each device
	for(unsigned int i = 0; i < ceLineDeviceMappingsPortOutputDeviceSize; ++i)
	{
		const CELineDeviceEntry& deviceEntry = ceLineDeviceMappingsPort[i];
		unsigned int deviceReturn = deviceEntry.device->CalculateCELineStatePortTransparent(location, data, ceLineState & deviceEntry.inputCELineMask, this, caller);
		ceLineState |= (deviceReturn & deviceEntry.outputCELineMask);
	}

	return ceLineState;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
BusInterface::MapEntry* BusInterface::ResolveMemoryAddress(unsigned int ce, unsigned int location) const
{
	if(usePhysicalMemoryMap)
	{
		//Resolve the address from the physical memory map
		for(const ThinList<MapEntry*>* i = physicalMemoryMap[location]; i != 0; i = i->next)
		{
			MapEntry* mapEntry = i->object;
			if(mapEntry->ce == (ce & mapEntry->ceMask))
			{
				return mapEntry;
			}
		}
	}
	else
	{
		//Resolve the address from the normal memory map
		for(unsigned int i = 0; i < (unsigned int)memoryMap.size(); ++i)
		{
			MapEntry* mapEntry = memoryMap[i];
			if((mapEntry->address <= (location & mapEntry->addressMask))	//The current memory map entry starts before or on the target address
				&& ((mapEntry->address + mapEntry->interfaceSize) > (location & mapEntry->addressMask))	//The current map entry ends after the target address
				&& (mapEntry->ce == (ce & mapEntry->ceMask)))	//The ce line mapping of the address mapping matches the ce line state of our query
			{
				return mapEntry;
			}
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
BusInterface::AccessResult BusInterface::ReadMemory(unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	AccessResult accessResult(false, true, 0);
	location &= addressBusMask;
	unsigned int ce = CalculateCELineStateMemory(location, data, caller, accessTime);
	MapEntry* mapEntry = ResolveMemoryAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			tempData = mapEntry->dataLineRemapTable.ConvertTo(data.GetData());
			accessResult = mapEntry->device->ReadInterface(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessTime, accessContext);
			data = mapEntry->dataLineRemapTable.ConvertFrom(tempData.GetData());

			//Generate the access mask for the data lines
			if(accessResult.accessMaskUsed)
			{
				//If a data access mask was specified, convert the access mask back using
				//the conversion table.
				accessResult.accessMask = mapEntry->dataLineRemapTable.ConvertFrom(accessResult.accessMask);
			}
			else
			{
				//If a data access mask wasn't specified, we generate one ourselves, using
				//the mask of the preserved lines in the source value. This is important,
				//since lines that are dropped due to data line remapping are obviously
				//masked.
				accessResult.accessMaskUsed = true;
				accessResult.accessMask = mapEntry->dataLineRemapTable.GetBitMaskOriginalLinesPreserved();
			}
		}
		else
		{
			accessResult = mapEntry->device->ReadInterface(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessTime, accessContext);
		}
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------
BusInterface::AccessResult BusInterface::WriteMemory(unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	AccessResult accessResult(false);
	location &= addressBusMask;
	unsigned int ce = CalculateCELineStateMemory(location, data, caller, accessTime);
	MapEntry* mapEntry = ResolveMemoryAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			tempData = mapEntry->dataLineRemapTable.ConvertTo(data.GetData());
			accessResult = mapEntry->device->WriteInterface(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessTime, accessContext);
		}
		else
		{
			accessResult = mapEntry->device->WriteInterface(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessTime, accessContext);
		}
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------
void BusInterface::TransparentReadMemory(unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext) const
{
	location &= addressBusMask;
	unsigned int ce = CalculateCELineStateMemoryTransparent(location, data, caller);
	MapEntry* mapEntry = ResolveMemoryAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			mapEntry->device->TransparentReadInterface(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessContext);
			data = mapEntry->dataLineRemapTable.ConvertFrom(tempData.GetData());
		}
		else
		{
			mapEntry->device->TransparentReadInterface(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessContext);
		}
	}
}

//----------------------------------------------------------------------------------------
void BusInterface::TransparentWriteMemory(unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext) const
{
	location &= addressBusMask;
	unsigned int ce = CalculateCELineStateMemoryTransparent(location, data, caller);
	MapEntry* mapEntry = ResolveMemoryAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			tempData = mapEntry->dataLineRemapTable.ConvertTo(data.GetData());
			mapEntry->device->TransparentWriteInterface(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessContext);
		}
		else
		{
			mapEntry->device->TransparentWriteInterface(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessContext);
		}
	}
}

//----------------------------------------------------------------------------------------
//Port interface functions
//----------------------------------------------------------------------------------------
BusInterface::MapEntry* BusInterface::ResolvePortAddress(unsigned int ce, unsigned int location) const
{
	if(usePhysicalPortMap)
	{
		//Resolve the address from the physical memory map
		for(const ThinList<MapEntry*>* i = physicalPortMap[location]; i != 0; i = i->next)
		{
			MapEntry* mapEntry = i->object;
			if(mapEntry->ce == (ce & mapEntry->ceMask))
			{
				return mapEntry;
			}
		}
	}
	else
	{
		//Resolve the address from the normal memory map
		for(unsigned int i = 0; i < (unsigned int)portMap.size(); ++i)
		{
			MapEntry* mapEntry = portMap[i];
			if((mapEntry->address <= (location & mapEntry->addressMask))	//The current memory map entry starts before or on the target address
				&& ((mapEntry->address + mapEntry->interfaceSize) > (location & mapEntry->addressMask))	//The current map entry ends after the target address
				&& (mapEntry->ce == (ce & mapEntry->ceMask)))	//The ce line mapping of the address mapping matches the ce line state of our query
			{
				return mapEntry;
			}
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
BusInterface::AccessResult BusInterface::ReadPort(unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	AccessResult accessResult(false, true, 0);
	location &= portAddressBusMask;
	unsigned int ce = CalculateCELineStatePort(location, data, caller, accessTime);
	MapEntry* mapEntry = ResolvePortAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			tempData = mapEntry->dataLineRemapTable.ConvertTo(data.GetData());
			accessResult = mapEntry->device->ReadPort(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessTime, accessContext);
			data = mapEntry->dataLineRemapTable.ConvertFrom(tempData.GetData());

			//Generate the access mask for the data lines
			if(accessResult.accessMaskUsed)
			{
				//If a data access mask was specified, convert the access mask back using
				//the conversion table.
				accessResult.accessMask = mapEntry->dataLineRemapTable.ConvertFrom(accessResult.accessMask);
			}
			else
			{
				//If a data access mask wasn't specified, we generate one ourselves, using
				//the mask of the preserved lines in the source value. This is important,
				//since lines that are dropped due to data line remapping are obviously
				//masked.
				accessResult.accessMaskUsed = true;
				accessResult.accessMask = mapEntry->dataLineRemapTable.GetBitMaskOriginalLinesPreserved();
			}
		}
		else
		{
			accessResult = mapEntry->device->ReadPort(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessTime, accessContext);
		}
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------
BusInterface::AccessResult BusInterface::WritePort(unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	AccessResult accessResult(false);
	location &= portAddressBusMask;
	unsigned int ce = CalculateCELineStatePort(location, data, caller, accessTime);
	MapEntry* mapEntry = ResolvePortAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			tempData = mapEntry->dataLineRemapTable.ConvertTo(data.GetData());
			accessResult = mapEntry->device->WritePort(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessTime, accessContext);
		}
		else
		{
			accessResult = mapEntry->device->WritePort(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessTime, accessContext);
		}
	}
	return accessResult;
}

//----------------------------------------------------------------------------------------
void BusInterface::TransparentReadPort(unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext) const
{
	location &= portAddressBusMask;
	unsigned int ce = CalculateCELineStatePortTransparent(location, data, caller);
	MapEntry* mapEntry = ResolvePortAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			mapEntry->device->TransparentReadPort(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessContext);
			data = mapEntry->dataLineRemapTable.ConvertFrom(tempData.GetData());
		}
		else
		{
			mapEntry->device->TransparentReadPort(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessContext);
		}
	}
}

//----------------------------------------------------------------------------------------
void BusInterface::TransparentWritePort(unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext) const
{
	location &= portAddressBusMask;
	unsigned int ce = CalculateCELineStatePortTransparent(location, data, caller);
	MapEntry* mapEntry = ResolvePortAddress(ce, location);
	if(mapEntry != 0)
	{
		unsigned int interfaceOffset;
		if(mapEntry->remapAddressLines)
		{
			//Remap address lines
			interfaceOffset = mapEntry->addressLineRemapTable.ConvertTo(location) + mapEntry->interfaceOffset;
		}
		else
		{
			interfaceOffset = ((location & mapEntry->addressMask) - mapEntry->address) + mapEntry->interfaceOffset;
		}

		if(mapEntry->remapDataLines)
		{
			//Remap data lines
			Data tempData(mapEntry->dataLineRemapTable.GetBitCountConverted());
			tempData = mapEntry->dataLineRemapTable.ConvertTo(data.GetData());
			mapEntry->device->TransparentWritePort(mapEntry->interfaceNumber, interfaceOffset, tempData, caller, accessContext);
		}
		else
		{
			mapEntry->device->TransparentWritePort(mapEntry->interfaceNumber, interfaceOffset, data, caller, accessContext);
		}
	}
}

//----------------------------------------------------------------------------------------
//Line interface functions
//----------------------------------------------------------------------------------------
bool BusInterface::SetLine(unsigned int sourceLine, const Data& lineData, IDeviceContext* sourceDevice, IDeviceContext* callingDevice, double accessTime, unsigned int accessContext)
{
	//##DEBUG##
//	std::wcout << "SetLineBegin\t" << sourceDevice->GetTargetDevice()->GetDeviceInstanceName() << '\t' << sourceLine << '\t' << sourceDevice->GetTargetDevice()->GetLineName(sourceLine) << '\n';
	for(std::list<LineEntry>::const_iterator i = lineMap.begin(); i != lineMap.end(); ++i)
	{
		const LineEntry* lineEntry = &(*i);
		//##DEBUG##
//		std::wcout << "SetLine:\t" << lineEntry->sourceDevice->GetDeviceInstanceName() << '\t' << lineEntry->targetDevice->GetDeviceInstanceName() << '\t' << lineEntry->sourceLine << '\t' << lineEntry->targetLine << '\n';
		if((lineEntry->sourceDevice == sourceDevice->GetTargetDevice()) && (lineEntry->sourceLine == sourceLine))
		{
			Data tempData(lineEntry->targetLineBitCount, lineData.GetData());
			if(lineEntry->remapLines)
			{
				//Remap lines
				tempData = lineEntry->lineRemapTable.ConvertTo(lineData.GetData());
			}
			lineEntry->targetDevice->SetLineState(lineEntry->targetLine, tempData, callingDevice, accessTime, accessContext);
		}
	}
	//##DEBUG##
//	std::wcout << "SetLineEnd\n";
	return true;
}
