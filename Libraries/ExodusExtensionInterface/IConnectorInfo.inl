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
	std::wstring result;
	GetConnectorClassNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetConnectorClassName(const std::wstring& aconnectorClassName)
{
	SetConnectorClassNameInternal(InteropSupport::STLObjectSource<std::wstring>(aconnectorClassName));
}

//----------------------------------------------------------------------------------------
std::wstring IConnectorInfo::GetExportingModuleConnectorInstanceName() const
{
	std::wstring result;
	GetExportingModuleConnectorInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetExportingModuleConnectorInstanceName(const std::wstring& aexportingModuleConnectorInstanceName)
{
	SetExportingModuleConnectorInstanceNameInternal(InteropSupport::STLObjectSource<std::wstring>(aexportingModuleConnectorInstanceName));
}

//----------------------------------------------------------------------------------------
std::wstring IConnectorInfo::GetSystemClassName() const
{
	std::wstring result;
	GetSystemClassNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetSystemClassName(const std::wstring& asystemClassName)
{
	SetSystemClassNameInternal(InteropSupport::STLObjectSource<std::wstring>(asystemClassName));
}

//----------------------------------------------------------------------------------------
//Importing module info
//----------------------------------------------------------------------------------------
std::wstring IConnectorInfo::GetImportingModuleConnectorInstanceName() const
{
	std::wstring result;
	GetImportingModuleConnectorInstanceNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
void IConnectorInfo::SetImportingModuleConnectorInstanceName(const std::wstring& aimportingModuleConnectorInstanceName)
{
	SetImportingModuleConnectorInstanceNameInternal(InteropSupport::STLObjectSource<std::wstring>(aimportingModuleConnectorInstanceName));
}
