//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemDefinition
{
	MenuItemDefinition(int amenuItemID, const std::wstring& amenuItemName, bool amenuItemOpensViewModel, bool ahiddenMenuItem = false)
	:menuItemID(amenuItemID), menuItemName(amenuItemName), menuItemOpensViewModel(amenuItemOpensViewModel), hiddenMenuItem(ahiddenMenuItem)
	{}

	int menuItemID;
	std::wstring menuItemName;
	bool menuItemOpensViewModel;
	bool hiddenMenuItem;
};

//----------------------------------------------------------------------------------------
struct MenuHandlerBase::MenuItemInternal
{
	MenuItemInternal(int amenuItemID, const std::wstring& amenuItemName, bool amenuItemOpensViewModel, bool ahiddenMenuItem)
	:viewModel(0), menuItemID(amenuItemID), menuItemName(amenuItemName), menuItemOpensViewModel(amenuItemOpensViewModel), hiddenMenuItem(ahiddenMenuItem)
	{}

	IViewModel* viewModel;
	int menuItemID;
	std::wstring menuItemName;
	bool menuItemOpensViewModel;
	bool hiddenMenuItem;
};
