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
	unsigned int arraySize = 1;
	std::vector<IMenuItem*> itemArray(arraySize);
	bool itemsRetrieved = false;
	do
	{
		unsigned int requiredArraySize;
		GetMenuItemsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
		itemArray.resize(requiredArraySize);
		arraySize = requiredArraySize;
	}
	while(!itemsRetrieved);

	//Load the set of items into our list structure, and return it to the caller.
	std::list<IMenuItem*> items;
	for(unsigned int i = 0; i < arraySize; ++i)
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
