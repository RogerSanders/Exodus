#include "DebugMenuHandler.h"
#include "MemoryEditorViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::DebugMenuHandler::DebugMenuHandler(MemoryRead* adevice)
:MenuHandlerBase(L"MemoryReadDebugMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void MemoryRead::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_MEMORYEDITOR, L"MemoryEditor", L"Memory Editor", true));
}

//----------------------------------------------------------------------------------------
IViewModel* MemoryRead::DebugMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_MEMORYEDITOR:
		return new MemoryEditorViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_MEMORYEDITOR, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MemoryRead::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
