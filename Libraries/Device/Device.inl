//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Device::Device(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:implementationName(aimplementationName), instanceName(ainstanceName), moduleID(amoduleID), deviceContext(0), assemblyHandle(0)
{}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring Device::GetDeviceImplementationName() const
{
	return implementationName;
}

//----------------------------------------------------------------------------------------
std::wstring Device::GetDeviceInstanceName() const
{
	return instanceName;
}

//----------------------------------------------------------------------------------------
std::wstring Device::GetFullyQualifiedDeviceInstanceName() const
{
	return deviceContext->GetFullyQualifiedDeviceInstanceName();
}

//----------------------------------------------------------------------------------------
std::wstring Device::GetModuleDisplayName() const
{
	return deviceContext->GetModuleDisplayName();
}

//----------------------------------------------------------------------------------------
std::wstring Device::GetModuleInstanceName() const
{
	return deviceContext->GetModuleInstanceName();
}
