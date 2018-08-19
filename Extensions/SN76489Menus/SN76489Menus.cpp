#include "SN76489Menus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489Menus::SN76489Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Extension(implementationName, instanceName, moduleID)
{}

//----------------------------------------------------------------------------------------
SN76489Menus::~SN76489Menus()
{
	//Delete all menu handlers
	for (std::map<IDevice*, DebugMenuHandler*>::const_iterator i = _debugMenuHandlers.begin(); i != _debugMenuHandlers.end(); ++i)
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
	if (targetDeviceAsISN76489 == 0)
	{
		return false;
	}

	//Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsISN76489);
	menuHandler->LoadMenuItems();
	_debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------
void SN76489Menus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = _debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	_debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void SN76489Menus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if (deviceMenu == IExtension::DeviceMenu::Debug)
	{
		_debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool SN76489Menus::RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
}

//----------------------------------------------------------------------------------------
bool SN76489Menus::OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}
