//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IConnectorInfo::ThisIConnectorInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Exporting module info
//----------------------------------------------------------------------------------------
std::wstring IConnectorInfo::GetConnectorClassName() const
{
	return GetConnectorClassNameInternal();
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetConnectorClassName(const std::wstring& aconnectorClassName)
{
	return SetConnectorClassNameInternal(aconnectorClassName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring IConnectorInfo::GetExportingModuleConnectorInstanceName() const
{
	return GetExportingModuleConnectorInstanceNameInternal();
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetExportingModuleConnectorInstanceName(const std::wstring& aexportingModuleConnectorInstanceName)
{
	SetExportingModuleConnectorInstanceNameInternal(aexportingModuleConnectorInstanceName.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring IConnectorInfo::GetSystemClassName() const
{
	return GetSystemClassNameInternal();
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetSystemClassName(const std::wstring& asystemClassName)
{
	SetSystemClassNameInternal(asystemClassName.c_str());
}

//----------------------------------------------------------------------------------------
//Importing module info
//----------------------------------------------------------------------------------------
std::wstring IConnectorInfo::GetImportingModuleConnectorInstanceName() const
{
	return GetImportingModuleConnectorInstanceNameInternal();
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetImportingModuleConnectorInstanceName(const std::wstring& aimportingModuleConnectorInstanceName)
{
	SetImportingModuleConnectorInstanceNameInternal(aimportingModuleConnectorInstanceName.c_str());
}
