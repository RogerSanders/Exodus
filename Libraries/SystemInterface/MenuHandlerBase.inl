//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemDefinition
{
	MenuItemDefinition(int amenuItemID, const std::wstring& amenuItemName, const std::wstring& amenuItemTitle, bool amenuItemOpensViewModel, bool ahiddenMenuItem = false)
	:menuItemID(amenuItemID), menuItemName(amenuItemName), menuItemTitle(amenuItemTitle), menuItemOpensViewModel(amenuItemOpensViewModel), hiddenMenuItem(ahiddenMenuItem)
	{}

	int menuItemID;
	std::wstring menuItemName;
	std::wstring menuItemTitle;
	bool menuItemOpensViewModel;
	bool hiddenMenuItem;
};

//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemInternal
{
	MenuItemInternal(int amenuItemID, const std::wstring& amenuItemName, const std::wstring& amenuItemTitle, bool amenuItemOpensViewModel, bool ahiddenMenuItem)
	:viewModel(0), menuItemID(amenuItemID), menuItemName(amenuItemName), menuItemTitle(amenuItemTitle), menuItemOpensViewModel(amenuItemOpensViewModel), hiddenMenuItem(ahiddenMenuItem)
	{}

	IViewModel* viewModel;
	int menuItemID;
	std::wstring menuItemName;
	std::wstring menuItemTitle;
	bool menuItemOpensViewModel;
	bool hiddenMenuItem;
};
