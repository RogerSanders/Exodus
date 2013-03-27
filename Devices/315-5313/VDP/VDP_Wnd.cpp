#include "VDP.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void VDP::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	if(menuHandler == 0)
	{
		menuHandler = new DebugMenuHandler(this);
		menuHandler->LoadMenuItems();
	}
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void VDP::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void VDP::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void VDP::OpenSpriteListDetailsView(unsigned int aspriteMappingAddress, unsigned int aspriteIndex)
{
	menuHandler->OpenSpriteListDetailsView(aspriteMappingAddress, aspriteIndex);
}

//----------------------------------------------------------------------------------------
void VDP::OpenPortMonitorDetailsView(const PortMonitorEntry& aentry)
{
	menuHandler->OpenPortMonitorDetailsView(aentry);
}
