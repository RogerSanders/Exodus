#include "YM2612Menus.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612Menus::YM2612Menus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Extension(aimplementationName, ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
YM2612Menus::~YM2612Menus()
{
	//Delete all menu handlers
	for(std::map<const IDevice*, DebugMenuHandler*>::const_iterator i = debugMenuHandlers.begin(); i != debugMenuHandlers.end(); ++i)
	{
		i->second->ClearMenuItems();
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
bool YM2612Menus::RegisterDeviceMenuHandler(IDevice* targetDevice)
{
	//Attempt to cast the supplied device to the correct type
	IYM2612* targetDeviceAsIYM2612 = dynamic_cast<IYM2612*>(targetDevice);
	if(targetDeviceAsIYM2612 == 0)
	{
		return false;
	}

	//Create a new menu handler for the target device
	DebugMenuHandler* menuHandler = new DebugMenuHandler(*this, *targetDevice, *targetDeviceAsIYM2612);
	menuHandler->LoadMenuItems();
	debugMenuHandlers[targetDevice] = menuHandler;
	return true;
}

//----------------------------------------------------------------------------------------
void YM2612Menus::UnregisterDeviceMenuHandler(IDevice* targetDevice)
{
	DebugMenuHandler* menuHandler = debugMenuHandlers[targetDevice];
	menuHandler->ClearMenuItems();
	delete menuHandler;
	debugMenuHandlers.erase(targetDevice);
}

//----------------------------------------------------------------------------------------
void YM2612Menus::AddDeviceMenuItems(DeviceMenu deviceMenu, IMenuSegment& menuSegment, IDevice* targetDevice)
{
	if(deviceMenu == IExtension::DeviceMenu::Debug)
	{
		debugMenuHandlers[targetDevice]->AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool YM2612Menus::RestoreDeviceViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
}

//----------------------------------------------------------------------------------------
bool YM2612Menus::OpenDeviceView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IDevice* targetDevice)
{
	return debugMenuHandlers[targetDevice]->OpenView(viewGroupName, viewName);
}

//----------------------------------------------------------------------------------------
void YM2612Menus::OpenOperatorView(const IDevice* targetDevice, unsigned int achannelNo, unsigned int aoperatorNo)
{
	debugMenuHandlers[targetDevice]->OpenOperatorView(achannelNo, aoperatorNo);
}
