#include "MemoryMenus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
MemoryMenus::MemoryMenus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Extension(implementationName, instanceName, moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
MemoryMenus::~MemoryMenus()
{
	// Delete all menu handlers
	for (std::map<IDevice*, DebugMenuHandler*>::const_iterator i = _debugMenuHandlers.begin(); i != _debugMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Window functions
//----------------------------------------------------------------------------------------------------------------------
bool MemoryMenus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	// Attempt to cast the supplied device to the correct type
	IMemory* targetDeviceAsIMemory = dynamic_cast<IMemory*>(targetDevice);
	if (targetDeviceAsIMemory == 0)
	{
		return false;
	}

	// Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIMemory);
	menuHandler->LoadMenuItems();
	_debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryMenus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = _debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	_debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryMenus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if (deviceMenu == IExtension::DeviceMenu::Debug)
	{
		_debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool MemoryMenus::RestoreDeviceViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
}

//----------------------------------------------------------------------------------------------------------------------
bool MemoryMenus::OpenDeviceView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	return _debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}
