//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemDefinition
{
	MenuItemDefinition(int amenuItemID, const std::wstring& amenuItemName, const std::wstring& amenuItemTitle, bool amenuItemOpensView, bool aallowOpenViewMultipleTimes = false, bool ahiddenMenuItem = false)
	:menuItemID(amenuItemID), menuItemName(amenuItemName), menuItemTitle(amenuItemTitle), menuItemOpensView(amenuItemOpensView), allowOpenViewMultipleTimes(aallowOpenViewMultipleTimes), hiddenMenuItem(ahiddenMenuItem)
	{}

	int menuItemID;
	std::wstring menuItemName;
	std::wstring menuItemTitle;
	bool menuItemOpensView;
	bool allowOpenViewMultipleTimes;
	bool hiddenMenuItem;
};

//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemInternal
{
	MenuItemInternal(MenuItemDefinition aitemDefinition)
	:itemDefinition(aitemDefinition)
	{}

	std::set<IViewPresenter*> viewPresenters;
	MenuItemDefinition itemDefinition;
};
