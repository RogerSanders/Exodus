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
bool IGUIExtensionInterface::LoadModuleFromFile(const std::wstring& fileDir, const std::wstring& fileName)
{
	return LoadModuleFromFileInternal(fileDir.c_str(), fileName.c_str());
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
