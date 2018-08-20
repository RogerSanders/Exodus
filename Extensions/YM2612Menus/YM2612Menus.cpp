#include "YM2612Menus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
YM2612Menus::YM2612Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Extension(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
YM2612Menus::~YM2612Menus()
{
	// Delete all menu handlers
	for (std::map<const IDevice*, DebugMenuHandler*>::const_iterator i = _debugMenuHandlers.begin(); i != _debugMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Window functions
//----------------------------------------------------------------------------------------------------------------------
bool YM2612Menus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	// Attempt to cast the supplied device to the correct type
	IYM2612* targetDeviceAsIYM2612 = dynamic_cast<IYM2612*>(targetDevice);
	if (targetDeviceAsIYM2612 == 0)
	{
		return false;
	}

	// Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIYM2612);
	menuHandler->LoadMenuItems();
	_debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void YM2612Menus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = _debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	_debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------------------------------------
void YM2612Menus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if (deviceMenu == IExtension::DeviceMenu::Debug)
	{
		_debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool YM2612Menus::RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
}

//----------------------------------------------------------------------------------------------------------------------
bool YM2612Menus::OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}

//----------------------------------------------------------------------------------------------------------------------
void YM2612Menus::OpenOperatorView(const IDevice* targetDevice, unsigned int channelNo, unsigned int operatorNo)
{
	_debugMenuHandlers[targetDevice]->OpenOperatorView(channelNo, operatorNo);
}
