#include "DebugMenuHandler.h"
#include "GenericDataViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(GenericAccessMenus& aowner, const IDevice& amodelInstanceKey, IGenericAccess& amodel)
:MenuHandlerBase(L"GenericAccessDebugMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	unsigned int pageCount = model.GetGenericAccessPageCount();
	for(unsigned int i = 0; i < pageCount; ++i)
	{
		const IGenericAccessPage* page = model.GetGenericAccessPage(i);
		menuItems.push_back(MenuItemDefinition((int)i, page->GetName(), page->GetTitle(), true, true));
	}
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	const IGenericAccessPage* page = model.GetGenericAccessPage((unsigned int)menuItemID);
	return new GenericDataViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model, page);
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
