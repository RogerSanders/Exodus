#include "ProcessorMenus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ProcessorMenus::ProcessorMenus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Extension(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
ProcessorMenus::~ProcessorMenus()
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
bool ProcessorMenus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	//Attempt to cast the supplied device to the correct type
	IProcessor* targetDeviceAsIProcessor = dynamic_cast<IProcessor*>(targetDevice);
	if(targetDeviceAsIProcessor == 0)
	{
		return false;
	}

	//Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIProcessor);
	menuHandler->LoadMenuItems();
	debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------
void ProcessorMenus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void ProcessorMenus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if(deviceMenu == IExtension::DeviceMenu::Debug)
	{
		debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool ProcessorMenus::RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
}

//----------------------------------------------------------------------------------------
bool ProcessorMenus::OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}
