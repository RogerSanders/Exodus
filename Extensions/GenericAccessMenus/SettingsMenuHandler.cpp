#include "SettingsMenuHandler.h"
#include "GenericDataViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SettingsMenuHandler::SettingsMenuHandler(GenericAccessMenus& aowner, const IDevice& amodelInstanceKey, IGenericAccess& amodel)
:MenuHandlerBase(L"GenericAccessSettingsMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void SettingsMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	unsigned int pageCount = model.GetGenericAccessPageCount();
	for(unsigned int i = 0; i < pageCount; ++i)
	{
		const IGenericAccessPage* page = model.GetGenericAccessPage(i);
		if(page->GetPageType() == IGenericAccessPage::Type::Settings)
		{
			menuItems.push_back(MenuItemDefinition((int)i, page->GetName(), page->GetTitle(), true, true));
		}
	}
}

//----------------------------------------------------------------------------------------
IViewPresenter* SettingsMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	const IGenericAccessPage* page = model.GetGenericAccessPage((unsigned int)menuItemID);
	return new GenericDataViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model, page);
}

//----------------------------------------------------------------------------------------
void SettingsMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
