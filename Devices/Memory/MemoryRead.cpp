#include "MemoryRead.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryRead(const std::wstring& aclassName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Device(aclassName, ainstanceName, amoduleID), interfaceSize(0)
{
	//Create the menu handler
	menuHandler = new DebugMenuHandler(this);
	menuHandler->LoadMenuItems();
}

//----------------------------------------------------------------------------------------
MemoryRead::~MemoryRead()
{
	//Delete the menu handler
	menuHandler->ClearMenuItems();
	delete menuHandler;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
bool MemoryRead::Construct(IHeirarchicalStorageNode& node)
{
	//Read interface size
	IHeirarchicalStorageAttribute* interfaceSizeAttribute = node.GetAttribute(L"InterfaceSize");
	if(interfaceSizeAttribute != 0)
	{
		SetInterfaceSize(interfaceSizeAttribute->ExtractHexValue<unsigned int>());
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Memory size functions
//----------------------------------------------------------------------------------------
unsigned int MemoryRead::GetInterfaceSize() const
{
	return interfaceSize;
}

//----------------------------------------------------------------------------------------
void MemoryRead::SetInterfaceSize(unsigned int ainterfaceSize)
{
	interfaceSize = ainterfaceSize;
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
bool MemoryRead::IsByteLocked(unsigned int location) const
{
	return false;
}
