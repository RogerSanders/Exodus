//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IMenuSegment::ThisIMenuSegmentVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Menu title functions
//----------------------------------------------------------------------------------------
std::wstring IMenuSegment::GetMenuSortTitle() const
{
	std::wstring result;
	GetMenuSortTitleInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Item management functions
//----------------------------------------------------------------------------------------
std::list<IMenuItem*> IMenuSegment::GetMenuItems() const
{
	std::list<IMenuItem*> result;
	GetMenuItemsInternal(InteropSupport::STLObjectTarget<std::list<IMenuItem*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::list<IMenuItem*> IMenuSegment::GetSortedMenuItems() const
{
	std::list<IMenuItem*> result;
	GetSortedMenuItemsInternal(InteropSupport::STLObjectTarget<std::list<IMenuItem*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Menu item creation and deletion
//----------------------------------------------------------------------------------------
IMenuSubmenu& IMenuSegment::AddMenuItemSubmenu(const std::wstring& title)
{
	return AddMenuItemSubmenuInternal(InteropSupport::STLObjectSource<std::wstring>(title));
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& IMenuSegment::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title)
{
	return AddMenuItemSelectableOptionInternal(menuHandler, menuItemID, InteropSupport::STLObjectSource<std::wstring>(title));
}
