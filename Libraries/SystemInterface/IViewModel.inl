//----------------------------------------------------------------------------------------
//View owner functions
//----------------------------------------------------------------------------------------
std::wstring IViewModel::GetViewOwnerDeviceInstanceName() const
{
	return GetViewOwnerDeviceInstanceNameInternal();
}

//----------------------------------------------------------------------------------------
void IViewModel::SetViewOwnerAsDevice(const std::wstring& deviceInstanceName, unsigned int moduleID)
{
	SetViewOwnerAsDeviceInternal(deviceInstanceName.c_str(), moduleID);
}

//----------------------------------------------------------------------------------------
//Title functions
//----------------------------------------------------------------------------------------
std::wstring IViewModel::GetViewTitle() const
{
	return GetViewTitleInternal();
}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
std::wstring IViewModel::GetMenuHandlerName() const
{
	return GetMenuHandlerNameInternal();
}
