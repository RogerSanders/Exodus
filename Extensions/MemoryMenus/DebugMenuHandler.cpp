#include "DebugMenuHandler.h"
#include "MemoryEditorViewPresenter.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(MemoryMenus& owner, const IDevice& modelInstanceKey, IMemory& model)
:MenuHandlerBase(L"MemoryReadDebugMenu", owner.GetViewManager()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// Management functions
//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_MEMORYEDITOR, L"MemoryEditor", MemoryEditorViewPresenter::GetUnqualifiedViewTitle(), true, true));
}

//----------------------------------------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch (menuItemID)
	{
	case MENUITEM_MEMORYEDITOR:
		return new MemoryEditorViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
