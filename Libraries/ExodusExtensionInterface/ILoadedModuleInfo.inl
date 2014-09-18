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
std::wstring ILoadedModuleInfo::GetModuleFilePath() const
{
	std::wstring result;
	GetModuleFilePathInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetModuleFilePath(const std::wstring& amoduleFilePath)
{
	SetModuleFilePathInternal(InteropSupport::STLObjectSource<std::wstring>(amoduleFilePath));
}

//----------------------------------------------------------------------------------------
//Required metadata
//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetSystemClassName() const
{
	std::wstring result;
	GetSystemClassNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetSystemClassName(const std::wstring& asystemClassName)
{
	SetSystemClassNameInternal(InteropSupport::STLObjectSource<std::wstring>(asystemClassName));
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetClassName() const
{
	std::wstring result;
	GetClassNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetClassName(const std::wstring& aclassName)
{
	SetClassNameInternal(InteropSupport::STLObjectSource<std::wstring>(aclassName));
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetInstanceName() const
{
	std::wstring result;
	GetInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetInstanceName(const std::wstring& ainstanceName)
{
	SetInstanceNameInternal(InteropSupport::STLObjectSource<std::wstring>(ainstanceName));
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetDisplayName() const
{
	std::wstring result;
	GetDisplayNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetDisplayName(const std::wstring& adisplayName)
{
	SetDisplayNameInternal(InteropSupport::STLObjectSource<std::wstring>(adisplayName));
}

//----------------------------------------------------------------------------------------
//Optional metadata
//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetProductionYear() const
{
	std::wstring result;
	GetProductionYearInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetProductionYear(const std::wstring& aproductionYear)
{
	SetProductionYearInternal(InteropSupport::STLObjectSource<std::wstring>(aproductionYear));
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetManufacturerCode() const
{
	std::wstring result;
	GetManufacturerCodeInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetManufacturerCode(const std::wstring& amanufacturerCode)
{
	SetManufacturerCodeInternal(InteropSupport::STLObjectSource<std::wstring>(amanufacturerCode));
}

//----------------------------------------------------------------------------------------
std::wstring ILoadedModuleInfo::GetManufacturerDisplayName() const
{
	std::wstring result;
	GetManufacturerDisplayNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void ILoadedModuleInfo::SetManufacturerDisplayName(const std::wstring& amanufacturerDisplayName)
{
	SetManufacturerDisplayNameInternal(InteropSupport::STLObjectSource<std::wstring>(amanufacturerDisplayName));
}
