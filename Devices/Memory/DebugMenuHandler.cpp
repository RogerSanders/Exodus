#include "DebugMenuHandler.h"
#include "MemoryEditorViewModel.h"
#include "MemoryViewerOldViewModel.h"
#include "MemoryEditorOldViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::DebugMenuHandler::DebugMenuHandler(MemoryRead* adevice)
:MenuHandlerBase(L"MemoryReadDebugMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void MemoryRead::DebugMenuHandler::OpenOldMemoryEditorView(unsigned int atargetMemoryAddress)
{
	IViewModel* viewModelBase = GetViewModelIfOpen(MENUITEM_MEMORYEDITOROLD);
	if(viewModelBase != 0)
	{
		MemoryEditorOldViewModel* viewModel = dynamic_cast<MemoryEditorOldViewModel*>(viewModelBase);
		if(viewModel != 0)
		{
			viewModel->SetTargetMemoryAddress(atargetMemoryAddress);
		}
	}
	else
	{
		viewModelBase = new MemoryEditorOldViewModel(GetMenuHandlerName(), GetMenuItemName(MENUITEM_MEMORYEDITOROLD), MENUITEM_MEMORYEDITOROLD, device, atargetMemoryAddress);
		if(!AddCreatedViewModel(MENUITEM_MEMORYEDITOROLD, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void MemoryRead::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_MEMORYEDITOR, L"MemoryEditor", L"Memory Editor", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_MEMORYVIEWEROLD, L"MemoryViewerOld", L"Old Memory Viewer", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_MEMORYEDITOROLD, L"MemoryEditorOld", L"Old Memory Editor", true));
}

//----------------------------------------------------------------------------------------
IViewModel* MemoryRead::DebugMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_MEMORYEDITOR:
		return new MemoryEditorViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_MEMORYEDITOR, device);
	case MENUITEM_MEMORYVIEWEROLD:
		return new MemoryViewerOldViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_MEMORYVIEWEROLD, device);
	case MENUITEM_MEMORYEDITOROLD:
		return new MemoryEditorOldViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_MEMORYEDITOROLD, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MemoryRead::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
