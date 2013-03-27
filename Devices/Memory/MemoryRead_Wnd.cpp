#include "MemoryRead.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void MemoryRead::CreateMenuHandlers()
{
	if(menuHandler == 0)
	{
		menuHandler = new DebugMenuHandler(this);
		menuHandler->LoadMenuItems();
	}
}

//----------------------------------------------------------------------------------------
void MemoryRead::AddDebugMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& viewModelLauncher)
{
	Device::AddDebugMenuItems(menuSegment, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->AddMenuItems(menuSegment, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void MemoryRead::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	Device::RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void MemoryRead::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	Device::OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
	CreateMenuHandlers();
	menuHandler->OpenViewModel(viewModelGroupName, viewModelName, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void MemoryRead::OpenOldMemoryEditorView(unsigned int targetMemoryLocation)
{
	menuHandler->OpenOldMemoryEditorView(targetMemoryLocation);
}
