#include "M68000.h"
#include "DebugMenuHandler.h"
namespace M68000 {

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void M68000::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Processor::AddDebugMenuItems(menuSegment, viewModelLauncher);
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void M68000::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Processor::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void M68000::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Processor::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

} //Close namespace M68000
