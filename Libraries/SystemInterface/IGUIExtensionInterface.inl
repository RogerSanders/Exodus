//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGUIExtensionInterface::ThisIGUIExtensionInterfaceVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Module functions
//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::CanModuleBeLoaded(const std::wstring& filePath) const
{
	return CanModuleBeLoadedInternal(filePath.c_str());
}

//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::LoadModuleFromFile(const std::wstring& filePath)
{
	return LoadModuleFromFileInternal(filePath.c_str());
}

//----------------------------------------------------------------------------------------
//Global preference functions
//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathModules() const
{
	return GetGlobalPreferencePathModulesInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathSavestates() const
{
	return GetGlobalPreferencePathSavestatesInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathPersistentState() const
{
	return GetGlobalPreferencePathPersistentStateInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathWorkspaces() const
{
	return GetGlobalPreferencePathWorkspacesInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathCaptures() const
{
	return GetGlobalPreferencePathCapturesInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathAssemblies() const
{
	return GetGlobalPreferencePathAssembliesInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferenceInitialSystem() const
{
	return GetGlobalPreferenceInitialSystemInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferenceInitialWorkspace() const
{
	return GetGlobalPreferenceInitialWorkspaceInternal();
}

//----------------------------------------------------------------------------------------
//Assembly functions
//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::LoadAssembly(const std::wstring& filePath)
{
	return LoadAssemblyInternal(filePath.c_str());
}

//----------------------------------------------------------------------------------------
//File selection functions
//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::SelectExistingFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, bool scanIntoArchives, std::wstring& selectedFilePath) const
{
	const wchar_t* selectedFilePathRawData = 0;
	if(!SelectExistingFileInternal(selectionTypeString.c_str(), defaultExtension.c_str(), initialFilePath.c_str(), initialDirectory.c_str(), scanIntoArchives, &selectedFilePathRawData))
	{
		return false;
	}
	selectedFilePath = selectedFilePathRawData;
	return true;
}

//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::SelectNewFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath) const
{
	const wchar_t* selectedFilePathRawData = 0;
	if(!SelectNewFileInternal(selectionTypeString.c_str(), defaultExtension.c_str(), initialFilePath.c_str(), initialDirectory.c_str(), &selectedFilePathRawData))
	{
		return false;
	}
	selectedFilePath = selectedFilePathRawData;
	return true;
}

//----------------------------------------------------------------------------------------
std::vector<std::wstring> IGUIExtensionInterface::PathSplitElements(const std::wstring& path) const
{
	//Obtain the set of items in an array
	unsigned int arraySize = 0;
	const wchar_t** itemArray = PathSplitElementsInternal(path.c_str(), arraySize);

	//Load the set of items into our list structure
	std::vector<std::wstring> items;
	for(unsigned int i = 0; i < arraySize; ++i)
	{
		items.push_back(itemArray[i]);
	}

	//Free the allocated array data
	PathSplitElementsInternalFreeArray(itemArray, arraySize);

	//Return the set of items to the caller
	return items;
}

//----------------------------------------------------------------------------------------
Stream::IStream* IGUIExtensionInterface::OpenExistingFileForRead(const std::wstring& path) const
{
	return OpenExistingFileForReadInternal(path.c_str());
}
