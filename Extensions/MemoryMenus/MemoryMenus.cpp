#include "MemoryMenus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryMenus::MemoryMenus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Extension(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
MemoryMenus::~MemoryMenus()
{
	//Delete all menu handlers
	for(std::map<IDevice*, DebugMenuHandler*>::const_iterator i = debugMenuHandlers.begin(); i != debugMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
bool MemoryMenus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	//Attempt to cast the supplied device to the correct type
	IMemory* targetDeviceAsIMemory = dynamic_cast<IMemory*>(targetDevice);
	if(targetDeviceAsIMemory == 0)
	{
		return false;
	}

	//Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIMemory);
	menuHandler->LoadMenuItems();
	debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------
void MemoryMenus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void MemoryMenus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if(deviceMenu == IExtension::DEVICEMENU_DEBUG)
	{
		debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool MemoryMenus::RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
}

//----------------------------------------------------------------------------------------
bool MemoryMenus::OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}
