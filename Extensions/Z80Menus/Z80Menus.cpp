#include "Z80Menus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Z80Menus::Z80Menus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Extension(implementationName, instanceName, moduleID)
{}

//----------------------------------------------------------------------------------------
Z80Menus::~Z80Menus()
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
bool Z80Menus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	//Attempt to cast the supplied device to the correct type
	IZ80* targetDeviceAsIZ80 = dynamic_cast<IZ80*>(targetDevice);
	if (targetDeviceAsIZ80 == 0)
	{
		return false;
	}

	//Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIZ80);
	menuHandler->LoadMenuItems();
	_debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------
void Z80Menus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = _debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	_debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void Z80Menus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if (deviceMenu == IExtension::DeviceMenu::Debug)
	{
		_debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool Z80Menus::RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
}

//----------------------------------------------------------------------------------------
bool Z80Menus::OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}
