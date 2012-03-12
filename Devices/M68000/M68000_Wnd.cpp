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
void M68000::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Processor::RestoreViewModelState(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
	menuHandler->RestoreMenuViewModelOpen(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void M68000::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	Processor::OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
	menuHandler->OpenViewModel(menuHandlerName, viewModelID, viewModelLauncher);
}

} //Close namespace M68000
