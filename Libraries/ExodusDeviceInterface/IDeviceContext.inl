//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IDeviceContext::ThisIDeviceContextVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//System message functions
//----------------------------------------------------------------------------------------
std::wstring IDeviceContext::GetFullyQualifiedDeviceInstanceName() const
{
	std::wstring result;
	GetFullyQualifiedDeviceInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IDeviceContext::GetModuleDisplayName() const
{
	std::wstring result;
	GetModuleDisplayNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IDeviceContext::GetModuleInstanceName() const
{
	std::wstring result;
	GetModuleInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
