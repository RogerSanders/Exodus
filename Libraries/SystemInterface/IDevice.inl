//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IDevice* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IExtension* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IClockSource* target)
{
	return AddReference(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetDeviceClassName() const
{
	return GetDeviceClassNameInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetDeviceInstanceName() const
{
	return GetDeviceInstanceNameInternal();
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetModuleDisplayName() const
{
	return GetModuleDisplayNameInternal();
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetCELineID(const std::wstring& lineName, bool inputLine) const
{
	return GetCELineID(lineName.c_str(), inputLine);
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetLineID(const std::wstring& lineName) const
{
	return GetLineID(lineName.c_str());
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetClockSourceID(const std::wstring& lineName) const
{
	return GetClockSourceID(lineName.c_str());
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetKeyCodeID(const std::wstring& keyCodeName) const
{
	return GetKeyCodeID(keyCodeName.c_str());
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void IDevice::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	RestoreViewModelState(menuHandlerName.c_str(), viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void IDevice::OpenViewModel(const std::wstring& menuHandlerName, int viewModelID, IViewModelLauncher& viewModelLauncher)
{
	OpenViewModel(menuHandlerName.c_str(), viewModelID, viewModelLauncher);
}
