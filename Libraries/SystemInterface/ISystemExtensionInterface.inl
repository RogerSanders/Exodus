//----------------------------------------------------------------------------------------
//Loaded module info functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemExtensionInterface::GetLoadedModuleIDs() const
{
	//Obtain the set of items in an array
	std::vector<unsigned int> itemArray;
	unsigned int arraySize = 0;
	unsigned int requiredArraySize = 1;
	bool itemsRetrieved = false;
	while(!itemsRetrieved)
	{
		arraySize = requiredArraySize;
		itemArray.resize(arraySize);
		GetLoadedModuleIDsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
	}

	//Load the set of items into our list structure, and return it to the caller.
	std::list<unsigned int> items;
	for(unsigned int i = 0; i < requiredArraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}
	return items;
}

//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemExtensionInterface::GetConnectorIDs() const
{
	//Obtain the set of items in an array
	std::vector<unsigned int> itemArray;
	unsigned int arraySize = 0;
	unsigned int requiredArraySize = 1;
	bool itemsRetrieved = false;
	while(!itemsRetrieved)
	{
		arraySize = requiredArraySize;
		itemArray.resize(arraySize);
		GetConnectorIDsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
	}

	//Load the set of items into our list structure, and return it to the caller.
	std::list<unsigned int> items;
	for(unsigned int i = 0; i < requiredArraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}
	return items;
}
