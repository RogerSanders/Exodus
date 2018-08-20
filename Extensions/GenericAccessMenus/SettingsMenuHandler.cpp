#include "SettingsMenuHandler.h"
#include "GenericDataViewPresenter.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
SettingsMenuHandler::SettingsMenuHandler(GenericAccessMenus& owner, const IDevice& modelInstanceKey, IGenericAccess& model)
:MenuHandlerBase(L"GenericAccessSettingsMenu", owner.GetViewManager()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Management functions
//----------------------------------------------------------------------------------------------------------------------
void SettingsMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	unsigned int pageCount = _model.GetGenericAccessPageCount();
	for (unsigned int i = 0; i < pageCount; ++i)
	{
		const IGenericAccessPage* page = _model.GetGenericAccessPage(i);
		if (page->GetPageType() == IGenericAccessPage::Type::Settings)
		{
			menuItems.push_back(MenuItemDefinition((int)i, page->GetName(), page->GetTitle(), true, true));
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
IViewPresenter* SettingsMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	const IGenericAccessPage* page = _model.GetGenericAccessPage((unsigned int)menuItemID);
	return new GenericDataViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model, page);
}

//----------------------------------------------------------------------------------------------------------------------
void SettingsMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
