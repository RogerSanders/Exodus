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
	return CanModuleBeLoadedInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}

//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::LoadModuleFromFile(const std::wstring& filePath)
{
	return LoadModuleFromFileInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}

//----------------------------------------------------------------------------------------
//Global preference functions
//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathModules() const
{
	std::wstring result;
	GetGlobalPreferencePathModulesInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathSavestates() const
{
	std::wstring result;
	GetGlobalPreferencePathSavestatesInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathPersistentState() const
{
	std::wstring result;
	GetGlobalPreferencePathPersistentStateInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathWorkspaces() const
{
	std::wstring result;
	GetGlobalPreferencePathWorkspacesInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathCaptures() const
{
	std::wstring result;
	GetGlobalPreferencePathCapturesInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferencePathAssemblies() const
{
	std::wstring result;
	GetGlobalPreferencePathAssembliesInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferenceInitialSystem() const
{
	std::wstring result;
	GetGlobalPreferenceInitialSystemInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGUIExtensionInterface::GetGlobalPreferenceInitialWorkspace() const
{
	std::wstring result;
	GetGlobalPreferenceInitialWorkspaceInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Assembly functions
//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::LoadAssembly(const std::wstring& filePath)
{
	return LoadAssemblyInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}

//----------------------------------------------------------------------------------------
//File selection functions
//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::SelectExistingFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, bool scanIntoArchives, std::wstring& selectedFilePath) const
{
	return SelectExistingFileInternal(InteropSupport::STLObjectSource<std::wstring>(selectionTypeString), InteropSupport::STLObjectSource<std::wstring>(defaultExtension), InteropSupport::STLObjectSource<std::wstring>(initialFilePath), InteropSupport::STLObjectSource<std::wstring>(initialDirectory), scanIntoArchives, InteropSupport::STLObjectTarget<std::wstring>(selectedFilePath));
}

//----------------------------------------------------------------------------------------
bool IGUIExtensionInterface::SelectNewFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath) const
{
	return SelectNewFileInternal(InteropSupport::STLObjectSource<std::wstring>(selectionTypeString), InteropSupport::STLObjectSource<std::wstring>(defaultExtension), InteropSupport::STLObjectSource<std::wstring>(initialFilePath), InteropSupport::STLObjectSource<std::wstring>(initialDirectory), InteropSupport::STLObjectTarget<std::wstring>(selectedFilePath));
}

//----------------------------------------------------------------------------------------
std::vector<std::wstring> IGUIExtensionInterface::PathSplitElements(const std::wstring& path) const
{
	std::vector<std::wstring> result;
	PathSplitElementsInternal(InteropSupport::STLObjectTarget<std::vector<std::wstring>>(result), InteropSupport::STLObjectSource<std::wstring>(path));
	return result;
}

//----------------------------------------------------------------------------------------
Stream::IStream* IGUIExtensionInterface::OpenExistingFileForRead(const std::wstring& path) const
{
	return OpenExistingFileForReadInternal(InteropSupport::STLObjectSource<std::wstring>(path));
}
