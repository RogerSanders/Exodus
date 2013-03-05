#include "S315_5313.h"
#include "SettingsMenuHandler.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void S315_5313::AddSettingsMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddSettingsMenuItems(menuSegment, viewModelLauncher);
	settingsMenuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	debugMenuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	settingsMenuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	debugMenuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	settingsMenuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	debugMenuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::OpenSpriteListDetailsView(unsigned int aspriteIndex)
{
	debugMenuHandler->OpenSpriteListDetailsView(aspriteIndex);
}

//----------------------------------------------------------------------------------------
void S315_5313::OpenPortMonitorDetailsView(const PortMonitorEntry& aentry)
{
	debugMenuHandler->OpenPortMonitorDetailsView(aentry);
}
