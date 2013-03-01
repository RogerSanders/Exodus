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
	//Obtain the set of items in an array
	std::vector<IMenuItem*> itemArray;
	unsigned int arraySize = 0;
	unsigned int requiredArraySize = 1;
	bool itemsRetrieved = false;
	while(!itemsRetrieved)
	{
		arraySize = requiredArraySize;
		itemArray.resize(arraySize);
		GetMenuItemsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
	}

	//Load the set of items into our list structure, and return it to the caller.
	std::list<IMenuItem*> items;
	for(unsigned int i = 0; i < requiredArraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}
	return items;
}

//----------------------------------------------------------------------------------------
//Menu item creation and deletion
//----------------------------------------------------------------------------------------
IMenuSubmenu& IMenuSegment::AddMenuItemSubmenu(const std::wstring& name)
{
	return AddMenuItemSubmenuInternal(name.c_str());
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& IMenuSegment::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name)
{
	return AddMenuItemSelectableOptionInternal(menuHandler, menuItemID, name.c_str());
}
