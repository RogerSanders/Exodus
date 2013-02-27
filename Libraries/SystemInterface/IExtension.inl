//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IDevice* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IExtension* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IExtension::AddReference(const std::wstring& referenceName, IClockSource* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
std::wstring IExtension::GetExtensionClassName() const
{
	return GetExtensionClassNameInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IExtension::GetExtensionInstanceName() const
{
	return GetExtensionInstanceNameInternal();
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void IExtension::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	RestoreViewModelState(menuHandlerName.c_str(), viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void IExtension::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	OpenViewModel(menuHandlerName.c_str(), viewModelID, viewModelLauncher);
}
