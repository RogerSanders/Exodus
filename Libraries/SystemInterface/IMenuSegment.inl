//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IMenuSegment::ThisIMenuSegmentVersion()
{
	return 1;
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
//Menu item creation and deletion
//----------------------------------------------------------------------------------------
IMenuSubmenu& IMenuSegment::AddMenuItemSubmenu(const std::wstring& name)
{
	return AddMenuItemSubmenuInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& IMenuSegment::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name)
{
	return AddMenuItemSelectableOptionInternal(menuHandler, menuItemID, InteropSupport::STLObjectSource<std::wstring>(name));
}
