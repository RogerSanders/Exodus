//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemDefinition
{
	MenuItemDefinition(int amenuItemID, const std::wstring& amenuItemName, const std::wstring& amenuItemTitle, bool amenuItemOpensView, bool ahiddenMenuItem = false)
	:menuItemID(amenuItemID), menuItemName(amenuItemName), menuItemTitle(amenuItemTitle), menuItemOpensView(amenuItemOpensView), hiddenMenuItem(ahiddenMenuItem)
	{}

	int menuItemID;
	std::wstring menuItemName;
	std::wstring menuItemTitle;
	bool menuItemOpensView;
	bool hiddenMenuItem;
};

//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemInternal
{
	MenuItemInternal(int amenuItemID, const std::wstring& amenuItemName, const std::wstring& amenuItemTitle, bool amenuItemOpensView, bool ahiddenMenuItem)
	:viewPresenter(0), menuItemID(amenuItemID), menuItemName(amenuItemName), menuItemTitle(amenuItemTitle), menuItemOpensView(amenuItemOpensView), hiddenMenuItem(ahiddenMenuItem)
	{}

	IViewPresenter* viewPresenter;
	int menuItemID;
	std::wstring menuItemName;
	std::wstring menuItemTitle;
	bool menuItemOpensView;
	bool hiddenMenuItem;
};
