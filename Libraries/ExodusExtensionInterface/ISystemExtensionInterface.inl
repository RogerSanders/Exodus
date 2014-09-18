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
	std::list<unsigned int> result;
	GetLoadedModuleIDsInternal(InteropSupport::STLObjectTarget<std::list<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
bool ISystemExtensionInterface::GetModuleDisplayName(unsigned int moduleID, std::wstring& moduleDisplayName) const
{
	return GetModuleDisplayNameInternal(moduleID, InteropSupport::STLObjectTarget<std::wstring>(moduleDisplayName));
}

//----------------------------------------------------------------------------------------
bool ISystemExtensionInterface::GetModuleInstanceName(unsigned int moduleID, std::wstring& moduleInstanceName) const
{
	return GetModuleInstanceNameInternal(moduleID, InteropSupport::STLObjectTarget<std::wstring>(moduleInstanceName));
}

//----------------------------------------------------------------------------------------
//Connector info functions
//----------------------------------------------------------------------------------------
std::list<unsigned int> ISystemExtensionInterface::GetConnectorIDs() const
{
	std::list<unsigned int> result;
	GetConnectorIDsInternal(InteropSupport::STLObjectTarget<std::list<unsigned int>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Loaded device info functions
//----------------------------------------------------------------------------------------
std::list<IDevice*> ISystemExtensionInterface::GetLoadedDevices() const
{
	std::list<IDevice*> result;
	GetLoadedDevicesInternal(InteropSupport::STLObjectTarget<std::list<IDevice*>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
bool ISystemExtensionInterface::GetDeviceDisplayName(IDevice* device, std::wstring& deviceDisplayName) const
{
	return GetDeviceDisplayNameInternal(device, InteropSupport::STLObjectTarget<std::wstring>(deviceDisplayName));
}

//----------------------------------------------------------------------------------------
bool ISystemExtensionInterface::GetDeviceInstanceName(IDevice* device, std::wstring& deviceInstanceName) const
{
	return GetDeviceInstanceNameInternal(device, InteropSupport::STLObjectTarget<std::wstring>(deviceInstanceName));
}

//----------------------------------------------------------------------------------------
bool ISystemExtensionInterface::GetFullyQualifiedDeviceDisplayName(IDevice* device, std::wstring& fullyQualifiedDeviceDisplayName) const
{
	return GetFullyQualifiedDeviceDisplayNameInternal(device, InteropSupport::STLObjectTarget<std::wstring>(fullyQualifiedDeviceDisplayName));
}

//----------------------------------------------------------------------------------------
//Loaded extension info functions
//----------------------------------------------------------------------------------------
std::list<IExtension*> ISystemExtensionInterface::GetLoadedExtensions() const
{
	std::list<IExtension*> result;
	GetLoadedExtensionsInternal(InteropSupport::STLObjectTarget<std::list<IExtension*>>(result));
	return result;
}
