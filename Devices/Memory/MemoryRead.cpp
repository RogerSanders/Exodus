#include "MemoryRead.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryRead(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aimplementationName, ainstanceName, amoduleID), menuHandler(0), memoryEntryCount(0)
{}

//----------------------------------------------------------------------------------------
MemoryRead::~MemoryRead()
{
	//Delete the menu handler
	if(menuHandler != 0)
	{
		menuHandler->ClearMenuItems();
		delete menuHandler;
	}
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool MemoryRead::Construct(IHierarchicalStorageNode& node)
{
	//Read interface size
	IHierarchicalStorageAttribute* memoryEntryCountAttribute = node.GetAttribute(L"MemoryEntryCount");
	if(memoryEntryCountAttribute != 0)
	{
		SetMemoryEntryCount(memoryEntryCountAttribute->ExtractHexValue<unsigned int>());
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Memory size functions
//----------------------------------------------------------------------------------------
unsigned int MemoryRead::GetMemoryEntryCount() const
{
	return memoryEntryCount;
}

//----------------------------------------------------------------------------------------
void MemoryRead::SetMemoryEntryCount(unsigned int amemoryEntryCount)
{
	memoryEntryCount = amemoryEntryCount;
}

//----------------------------------------------------------------------------------------
//Memory locking functions
//----------------------------------------------------------------------------------------
bool MemoryRead::IsMemoryLockingSupported() const
{
	return false;
}

//----------------------------------------------------------------------------------------
void MemoryRead::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{}

//----------------------------------------------------------------------------------------
bool MemoryRead::IsAddressLocked(unsigned int location) const
{
	return false;
}
