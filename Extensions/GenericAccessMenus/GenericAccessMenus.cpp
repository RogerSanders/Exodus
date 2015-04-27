#include "GenericAccessMenus.h"
#include "DebugMenuHandler.h"
#include "SettingsMenuHandler.h"

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
	for(std::map<IDevice*, SettingsMenuHandler*>::const_iterator i = settingsMenuHandlers.begin(); i != settingsMenuHandlers.end(); ++i)
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

	//Create a new debug menu handler for the target device
	DebugMenuHandler* debugMenuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIDeviceGenericAccess);
	debugMenuHandler->LoadMenuItems();
	debugMenuHandlers[targetDevice] = debugMenuHandler;

	//Create a new settings menu handler for the target device
	SettingsMenuHandler* settingsMenuHandler = new SettingsMenuHandler(*this, *targetDevice, *targetDeviceAsIDeviceGenericAccess);
	settingsMenuHandler->LoadMenuItems();
	settingsMenuHandlers[targetDevice] = settingsMenuHandler;

	return true;
}

//----------------------------------------------------------------------------------------
void GenericAccessMenus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	//Delete the debug menu handler for the target device
	DebugMenuHandler* debugMenuHandler = debugMenuHandlers[targetDevice];
	debugMenuHandler->ClearMenuItems();
	delete debugMenuHandler;
	debugMenuHandlers.erase(targetDevice);

	//Delete the settings menu handler for the target device
	SettingsMenuHandler* settingsMenuHandler = settingsMenuHandlers[targetDevice];
	settingsMenuHandler->ClearMenuItems();
	delete settingsMenuHandler;
	settingsMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void GenericAccessMenus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	switch(deviceMenu)
	{
	case IExtension::DeviceMenu::Debug:
		debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
		break;
	case IExtension::DeviceMenu::Settings:
		settingsMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
		break;
	}
}

//----------------------------------------------------------------------------------------
bool GenericAccessMenus::RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	bool result = false;
	result |= debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	result |= settingsMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	return result;
}

//----------------------------------------------------------------------------------------
bool GenericAccessMenus::OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	bool result = false;
	result |= debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
	result |= settingsMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
	return result;
}
