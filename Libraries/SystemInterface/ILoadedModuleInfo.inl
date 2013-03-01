//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ILoadedModuleInfo::ThisILoadedModuleInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//External information
//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetModuleFileDirectory() const
{
	return GetModuleFileDirectoryInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetModuleFileDirectory(const std::wstring& amoduleFileDirectory)
{
	SetModuleFileDirectoryInternal(amoduleFileDirectory.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetModuleFileName() const
{
	return GetModuleFileNameInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetModuleFileName(const std::wstring& amoduleFileName)
{
	SetModuleFileNameInternal(amoduleFileName.c_str());
}

//----------------------------------------------------------------------------------------
//Required metadata
//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetSystemClassName() const
{
	return GetSystemClassNameInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetSystemClassName(const std::wstring& asystemClassName)
{
	SetSystemClassNameInternal(asystemClassName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetClassName() const
{
	return GetClassNameInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetClassName(const std::wstring& aclassName)
{
	SetClassNameInternal(aclassName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetInstanceName() const
{
	return GetInstanceNameInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetInstanceName(const std::wstring& ainstanceName)
{
	SetInstanceNameInternal(ainstanceName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetDisplayName() const
{
	return GetDisplayNameInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetDisplayName(const std::wstring& adisplayName)
{
	SetDisplayNameInternal(adisplayName.c_str());
}

//----------------------------------------------------------------------------------------
//Optional metadata
//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetProductionYear() const
{
	return GetProductionYearInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetProductionYear(const std::wstring& aproductionYear)
{
	SetProductionYearInternal(aproductionYear.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetManufacturerCode() const
{
	return GetManufacturerCodeInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetManufacturerCode(const std::wstring& amanufacturerCode)
{
	SetManufacturerCodeInternal(amanufacturerCode.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetManufacturerDisplayName() const
{
	return GetManufacturerDisplayNameInternal();
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetManufacturerDisplayName(const std::wstring& amanufacturerDisplayName)
{
	SetManufacturerDisplayNameInternal(amanufacturerDisplayName.c_str());
}
