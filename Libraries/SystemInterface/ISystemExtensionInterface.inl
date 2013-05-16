//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ISystemExtensionInterface::ThisISystemExtensionInterfaceVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Loaded module info functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemExtensionInterface::GetLoadedModuleIDs() const
{
	//Obtain the set of items in an array
	unsigned int arraySize = 1;
	std::vector<unsigned int> itemArray(arraySize);
	bool itemsRetrieved = false;
	do
	{
		unsigned int requiredArraySize;
		GetLoadedModuleIDsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
		itemArray.resize(requiredArraySize);
		arraySize = requiredArraySize;
	}
	while(!itemsRetrieved);

	//Load the set of items into our list structure, and return it to the caller.
	std::list<unsigned int> items;
	for(unsigned int i = 0; i < arraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}
	return items;
}

//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemExtensionInterface::GetConnectorIDs() const
{
	//Obtain the set of items in an array
	unsigned int arraySize = 1;
	std::vector<unsigned int> itemArray(arraySize);
	bool itemsRetrieved = false;
	do
	{
		unsigned int requiredArraySize;
		GetConnectorIDsInternal(&itemArray[0], arraySize, requiredArraySize, itemsRetrieved);
		itemArray.resize(requiredArraySize);
		arraySize = requiredArraySize;
	}
	while(!itemsRetrieved);

	//Load the set of items into our list structure, and return it to the caller.
	std::list<unsigned int> items;
	for(unsigned int i = 0; i < arraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}
	return items;
}
