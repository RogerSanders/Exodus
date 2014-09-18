//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Device::Device(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:implementationName(aimplementationName), instanceName(ainstanceName), moduleID(amoduleID)
{}

//----------------------------------------------------------------------------------------
//Interface functions
//----------------------------------------------------------------------------------------
ISystemDeviceInterface& Device::GetSystemInterface() const
{
	return *systemInterface;
}

//----------------------------------------------------------------------------------------
//Device context functions
//----------------------------------------------------------------------------------------
IDeviceContext* Device::GetDeviceContext() const
{
	return deviceContext;
}

//----------------------------------------------------------------------------------------
double Device::GetCurrentTimesliceProgress() const
{
	return deviceContext->GetCurrentTimesliceProgress();
}

//----------------------------------------------------------------------------------------
void Device::SetCurrentTimesliceProgress(double executionProgress)
{
	deviceContext->SetCurrentTimesliceProgress(executionProgress);
}

//----------------------------------------------------------------------------------------
//Name functions
//----------------------------------------------------------------------------------------
std::wstring Device::GetDeviceClassName() const
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
