//----------------------------------------------------------------------------------------
//Exporting module info
//----------------------------------------------------------------------------------------
std::wstring ConnectorInfo::GetConnectorClassName() const
{
	return connectorClassName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetConnectorClassName(const std::wstring& aconnectorClassName)
{
	connectorClassName = aconnectorClassName;
}

//----------------------------------------------------------------------------------------
std::wstring ConnectorInfo::GetExportingModuleConnectorInstanceName() const
{
	return exportingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetExportingModuleConnectorInstanceName(const std::wstring& aexportingModuleConnectorInstanceName)
{
	exportingModuleConnectorInstanceName = aexportingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------
std::wstring ConnectorInfo::GetSystemClassName() const
{
	return systemClassName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetSystemClassName(const std::wstring& asystemClassName)
{
	systemClassName = asystemClassName;
}

//----------------------------------------------------------------------------------------
//Importing module info
//----------------------------------------------------------------------------------------
std::wstring ConnectorInfo::GetImportingModuleConnectorInstanceName() const
{
	return importingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetImportingModuleConnectorInstanceName(const std::wstring& aimportingModuleConnectorInstanceName)
{
	importingModuleConnectorInstanceName = aimportingModuleConnectorInstanceName;
}
