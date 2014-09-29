#include "SN76489Menus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489Menus::SN76489Menus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Extension(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
SN76489Menus::~SN76489Menus()
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
bool SN76489Menus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	//Attempt to cast the supplied device to the correct type
	ISN76489* targetDeviceAsISN76489 = dynamic_cast<ISN76489*>(targetDevice);
	if(targetDeviceAsISN76489 == 0)
	{
		return false;
	}

	//Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsISN76489);
	menuHandler->LoadMenuItems();
	debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------
void SN76489Menus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void SN76489Menus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if(deviceMenu == IExtension::DeviceMenu::Debug)
	{
		debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool SN76489Menus::RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
}

//----------------------------------------------------------------------------------------
bool SN76489Menus::OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}
