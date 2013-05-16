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
	unsigned int arraySize = 1;
	std::vector<IMenuSegment*> itemArray(arraySize);
	bool itemsRetrieved = false;
	do
	{
		unsigned int requiredArraySize;
		GetMenuSegmentsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
		itemArray.resize(requiredArraySize);
		arraySize = requiredArraySize;
	}
	while(!itemsRetrieved);

	//Load the set of items into our list structure, and return it to the caller.
	std::list<IMenuSegment*> items;
	for(unsigned int i = 0; i < arraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}
	return items;
}
