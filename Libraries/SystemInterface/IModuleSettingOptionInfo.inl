//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IModuleSettingOptionInfo::ThisIModuleSettingOptionInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
std::wstring IModuleSettingOptionInfo::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IModuleSettingOptionInfo::GetDisplayName() const
{
	std::wstring result;
	GetDisplayNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void IModuleSettingOptionInfo::SetName(const std::wstring& name)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
void IModuleSettingOptionInfo::SetDisplayName(const std::wstring& displayName)
{
	SetDisplayNameInternal(InteropSupport::STLObjectSource<std::wstring>(displayName));
}
