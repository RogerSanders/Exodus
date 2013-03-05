#include "Z80.h"
#include "DebugMenuHandler.h"
namespace Z80{

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void Z80::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Processor::AddDebugMenuItems(menuSegment, viewModelLauncher);
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Z80::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Processor::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Z80::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Processor::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

} //Close namespace Z80
