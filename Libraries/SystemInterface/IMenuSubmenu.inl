//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IMenuSubmenu::ThisIMenuSubmenuVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Menu name functions
//----------------------------------------------------------------------------------------
std::wstring IMenuSubmenu::GetMenuName() const
{
	return GetMenuNameInternal();
}

//----------------------------------------------------------------------------------------
//Menu segment functions
//----------------------------------------------------------------------------------------
std::list<IMenuSegment*> IMenuSubmenu::GetMenuSegments() const
{
	//Obtain the set of items in an array
	std::vector<IMenuSegment*> itemArray;
	unsigned int arraySize = 0;
	unsigned int requiredArraySize = 1;
	bool itemsRetrieved = false;
	while(!itemsRetrieved)
	{
		arraySize = requiredArraySize;
		itemArray.resize(arraySize);
		GetMenuSegmentsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
	}

	//Load the set of items into our list structure, and return it to the caller.
	std::list<IMenuSegment*> items;
	for(unsigned int i = 0; i < requiredArraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}
	return items;
}
