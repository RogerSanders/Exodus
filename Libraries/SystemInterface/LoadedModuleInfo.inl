//----------------------------------------------------------------------------------------
//External information
//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetModuleFilePath() const
{
	return filePath;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleFilePath(const std::wstring& amoduleFilePath)
{
	filePath = amoduleFilePath;
}

//----------------------------------------------------------------------------------------
//Required metadata
//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetSystemClassName() const
{
	return systemClassName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetSystemClassName(const std::wstring& asystemClassName)
{
	systemClassName = asystemClassName;
}

//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetClassName(const std::wstring& aclassName)
{
	className = aclassName;
}

//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetInstanceName() const
{
	return instanceName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetInstanceName(const std::wstring& ainstanceName)
{
	instanceName = ainstanceName;
}

//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetDisplayName() const
{
	return displayName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetDisplayName(const std::wstring& adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
//Optional metadata
//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetProductionYear() const
{
	return productionYear;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetProductionYear(const std::wstring& aproductionYear)
{
	productionYear = aproductionYear;
}

//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetManufacturerCode() const
{
	return manufacturerCode;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerCode(const std::wstring& amanufacturerCode)
{
	manufacturerCode = amanufacturerCode;
}

//----------------------------------------------------------------------------------------
std::wstring LoadedModuleInfo::GetManufacturerDisplayName() const
{
	return manufacturerDisplayName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerDisplayName(const std::wstring& amanufacturerDisplayName)
{
	manufacturerDisplayName = amanufacturerDisplayName;
}
