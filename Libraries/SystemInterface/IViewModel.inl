//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IViewModel::ThisIViewModelVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//View owner functions
//----------------------------------------------------------------------------------------
std::wstring IViewModel::GetViewOwnerDeviceInstanceName() const
{
	std::wstring result;
	GetViewOwnerDeviceInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IViewModel::SetViewOwnerAsDevice(const std::wstring& deviceInstanceName, unsigned int moduleID)
{
	SetViewOwnerAsDeviceInternal(InteropSupport::STLObjectSource<std::wstring>(deviceInstanceName), moduleID);
}

//----------------------------------------------------------------------------------------
//Title functions
//----------------------------------------------------------------------------------------
std::wstring IViewModel::GetViewTitle() const
{
	std::wstring result;
	GetViewTitleInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
std::wstring IViewModel::GetViewModelGroupName() const
{
	std::wstring result;
	GetViewModelGroupNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IViewModel::GetViewModelName() const
{
	std::wstring result;
	GetViewModelNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
