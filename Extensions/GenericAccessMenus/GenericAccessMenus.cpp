#include "GenericAccessMenus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessMenus::GenericAccessMenus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Extension(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
GenericAccessMenus::~GenericAccessMenus()
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
bool GenericAccessMenus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	//Attempt to cast the supplied device to the correct type
	IGenericAccess* targetDeviceAsIDeviceGenericAccess = dynamic_cast<IGenericAccess*>(targetDevice);
	if(targetDeviceAsIDeviceGenericAccess == 0)
	{
		return false;
	}

	//Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIDeviceGenericAccess);
	menuHandler->LoadMenuItems();
	debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------
void GenericAccessMenus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void GenericAccessMenus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if(deviceMenu == IExtension::DeviceMenu::Debug)
	{
		debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool GenericAccessMenus::RestoreDeviceViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
}

//----------------------------------------------------------------------------------------
bool GenericAccessMenus::OpenDeviceView(const std::wstring& viewGroupName, const std::wstring& viewName, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}
