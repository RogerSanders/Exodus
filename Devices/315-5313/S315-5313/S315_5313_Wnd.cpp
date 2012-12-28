#include "S315_5313.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void S315_5313::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
	menuHandler->RestoreMenuViewModelOpen(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
	menuHandler->OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void S315_5313::OpenSpriteListDetailsView(unsigned int aspriteIndex)
{
	menuHandler->OpenSpriteListDetailsView(aspriteIndex);
}

//----------------------------------------------------------------------------------------
void S315_5313::OpenPortMonitorDetailsView(const PortMonitorEntry& aentry)
{
	menuHandler->OpenPortMonitorDetailsView(aentry);
}
