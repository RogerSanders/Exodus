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
