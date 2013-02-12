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
void S315_5313::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
	settingsMenuHandler->RestoreMenuViewModelOpen(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
	debugMenuHandler->RestoreMenuViewModelOpen(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
	settingsMenuHandler->OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
	debugMenuHandler->OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
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
