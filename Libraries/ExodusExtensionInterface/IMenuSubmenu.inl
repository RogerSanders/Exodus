//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IMenuSubmenu::ThisIMenuSubmenuVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Menu title functions
//----------------------------------------------------------------------------------------
std::wstring IMenuSubmenu::GetMenuTitle() const
{
	std::wstring result;
	GetMenuTitleInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Menu segment functions
//----------------------------------------------------------------------------------------
std::list<IMenuItem*> IMenuSubmenu::GetMenuItems() const
{
	std::list<IMenuItem*> result;
	GetMenuItemsInternal(InteropSupport::STLObjectTarget<std::list<IMenuItem*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Menu item creation and deletion
//----------------------------------------------------------------------------------------
IMenuSubmenu& IMenuSubmenu::AddMenuItemSubmenu(const std::wstring& title)
{
	return AddMenuItemSubmenuInternal(InteropSupport::STLObjectSource<std::wstring>(title));
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& IMenuSubmenu::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title)
{
	return AddMenuItemSelectableOptionInternal(menuHandler, menuItemID, InteropSupport::STLObjectSource<std::wstring>(title));
}
