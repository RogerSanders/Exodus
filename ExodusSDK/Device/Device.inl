//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
Device::Device(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:_deviceContext(nullptr), _implementationName(implementationName), _instanceName(instanceName), _moduleID(moduleID)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------------------------
ISystemDeviceInterface& Device::GetSystemInterface() const
{
	return *_systemInterface;
}

//----------------------------------------------------------------------------------------------------------------------
// Device context functions
//----------------------------------------------------------------------------------------------------------------------
void Device::SetCurrentTimesliceProgress(double executionProgress)
{
	_deviceContext->SetCurrentTimesliceProgress(executionProgress);
}
