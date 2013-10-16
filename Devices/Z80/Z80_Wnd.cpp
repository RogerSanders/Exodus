#include "Z80.h"
#include "DebugMenuHandler.h"
namespace Z80{

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void Z80::CreateMenuHandlers()
{
	if(menuHandler == 0)
	{
		menuHandler = new DebugMenuHandler(this);
		menuHandler->LoadMenuItems();
	}
}

//----------------------------------------------------------------------------------------
void Z80::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Processor::AddDebugMenuItems(menuSegment, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Z80::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Processor::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void Z80::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Processor::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

} //Close namespace Z80
