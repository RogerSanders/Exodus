//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IModuleSettingInfo::ThisIModuleSettingInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
std::wstring IModuleSettingInfo::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IModuleSettingInfo::GetDisplayName() const
{
	std::wstring result;
	GetDisplayNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void IModuleSettingInfo::SetName(const std::wstring& name)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
void IModuleSettingInfo::SetDisplayName(const std::wstring& displayName)
{
	SetDisplayNameInternal(InteropSupport::STLObjectSource<std::wstring>(displayName));
}
