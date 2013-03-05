//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IDevice::UpdateMethod
{
	UPDATEMETHOD_NONE,
	UPDATEMETHOD_STEP,
	UPDATEMETHOD_TIMESLICE
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::ThisIDeviceVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Reference functions
//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IDevice* target)
{
	return AddReferenceInternal(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IExtension* target)
{
	return AddReferenceInternal(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	return AddReferenceInternal(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IClockSource* target)
{
	return AddReferenceInternal(referenceName.c_str(), target);
}

//----------------------------------------------------------------------------------------
//Name functions
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
	return GetCELineIDInternal(lineName.c_str(), inputLine);
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetLineID(const std::wstring& lineName) const
{
	return GetLineIDInternal(lineName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetLineName(unsigned int lineID) const
{
	return GetLineNameInternal(lineID);
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetClockSourceID(const std::wstring& lineName) const
{
	return GetClockSourceIDInternal(lineName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetClockSourceName(unsigned int clockSourceID) const
{
	return GetClockSourceNameInternal(clockSourceID);
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetKeyCodeID(const std::wstring& keyCodeName) const
{
	return GetKeyCodeIDInternal(keyCodeName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetKeyCodeName(unsigned int keyCodeID) const
{
	return GetKeyCodeNameInternal(keyCodeID);
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void IDevice::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	RestoreViewModelStateInternal(viewModelGroupName.c_str(), viewModelName.c_str(), node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void IDevice::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	OpenViewModelInternal(viewModelGroupName.c_str(), viewModelName.c_str(), viewModelLauncher);
}
