#include "GenericAccessMenus.h"
#include "DebugMenuHandler.h"
#include "SettingsMenuHandler.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
GenericAccessMenus::GenericAccessMenus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Extension(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
GenericAccessMenus::~GenericAccessMenus()
{
	// Delete all menu handlers
	for (std::map<IDevice*, DebugMenuHandler*>::const_iterator i = _debugMenuHandlers.begin(); i != _debugMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
	for (std::map<IDevice*, SettingsMenuHandler*>::const_iterator i = _settingsMenuHandlers.begin(); i != _settingsMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Window functions
//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessMenus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	// Attempt to cast the supplied device to the correct type
	IGenericAccess* targetDeviceAsIDeviceGenericAccess = dynamic_cast<IGenericAccess*>(targetDevice);
	if (targetDeviceAsIDeviceGenericAccess == 0)
	{
		return false;
	}

	// Create a new debug menu handler for the target device
	DebugMenuHandler* debugMenuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIDeviceGenericAccess);
	debugMenuHandler->LoadMenuItems();
	_debugMenuHandlers[targetDevice] = debugMenuHandler;

	// Create a new settings menu handler for the target device
	SettingsMenuHandler* settingsMenuHandler = new SettingsMenuHandler(*this, *targetDevice, *targetDeviceAsIDeviceGenericAccess);
	settingsMenuHandler->LoadMenuItems();
	_settingsMenuHandlers[targetDevice] = settingsMenuHandler;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessMenus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	// Delete the debug menu handler for the target device
	DebugMenuHandler* debugMenuHandler = _debugMenuHandlers[targetDevice];
	debugMenuHandler->ClearMenuItems();
	delete debugMenuHandler;
	_debugMenuHandlers.erase(targetDevice);

	// Delete the settings menu handler for the target device
	SettingsMenuHandler* settingsMenuHandler = _settingsMenuHandlers[targetDevice];
	settingsMenuHandler->ClearMenuItems();
	delete settingsMenuHandler;
	_settingsMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessMenus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	switch (deviceMenu)
	{
	case IExtension::DeviceMenu::Debug:
		_debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
		break;
	case IExtension::DeviceMenu::Settings:
		_settingsMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
		break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessMenus::RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	bool result = false;
	result |= _debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	result |= _settingsMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessMenus::OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	bool result = false;
	result |= _debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
	result |= _settingsMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
	return result;
}
