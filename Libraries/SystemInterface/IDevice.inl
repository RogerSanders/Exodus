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
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IExtension* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IBusInterface* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
bool IDevice::AddReference(const std::wstring& referenceName, IClockSource* target)
{
	return AddReferenceInternal(InteropSupport::STLObjectSource<std::wstring>(referenceName), target);
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring IDevice::GetDeviceImplementationName() const
{
	std::wstring result;
	GetDeviceImplementationNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetDeviceInstanceName() const
{
	std::wstring result;
	GetDeviceInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetFullyQualifiedDeviceInstanceName() const
{
	std::wstring result;
	GetFullyQualifiedDeviceInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetModuleDisplayName() const
{
	std::wstring result;
	GetModuleDisplayNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetModuleInstanceName() const
{
	std::wstring result;
	GetModuleInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//CE line state functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetCELineID(const std::wstring& lineName, bool inputLine) const
{
	return GetCELineIDInternal(InteropSupport::STLObjectSource<std::wstring>(lineName), inputLine);
}

//----------------------------------------------------------------------------------------
//Line functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetLineID(const std::wstring& lineName) const
{
	return GetLineIDInternal(InteropSupport::STLObjectSource<std::wstring>(lineName));
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetLineName(unsigned int lineID) const
{
	std::wstring result;
	GetLineNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result), lineID);
	return result;
}

//----------------------------------------------------------------------------------------
//Clock source functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetClockSourceID(const std::wstring& lineName) const
{
	return GetClockSourceIDInternal(InteropSupport::STLObjectSource<std::wstring>(lineName));
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetClockSourceName(unsigned int clockSourceID) const
{
	std::wstring result;
	GetClockSourceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result), clockSourceID);
	return result;
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
unsigned int IDevice::GetKeyCodeID(const std::wstring& keyCodeName) const
{
	return GetKeyCodeIDInternal(InteropSupport::STLObjectSource<std::wstring>(keyCodeName));
}

//----------------------------------------------------------------------------------------
std::wstring IDevice::GetKeyCodeName(unsigned int keyCodeID) const
{
	std::wstring result;
	GetKeyCodeNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result), keyCodeID);
	return result;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void IDevice::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	RestoreViewModelStateInternal(InteropSupport::STLObjectSource<std::wstring>(viewModelGroupName), InteropSupport::STLObjectSource<std::wstring>(viewModelName), node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void IDevice::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& viewModelLauncher)
{
	OpenViewModelInternal(InteropSupport::STLObjectSource<std::wstring>(viewModelGroupName), InteropSupport::STLObjectSource<std::wstring>(viewModelName), viewModelLauncher);
}
