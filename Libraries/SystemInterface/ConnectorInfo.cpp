#include "ConnectorInfo.h"

//----------------------------------------------------------------------------------------
//Internal data
//----------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetConnectorID() const
{
	return connectorID;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetConnectorID(unsigned int aconnectorID)
{
	connectorID = aconnectorID;
}

//----------------------------------------------------------------------------------------
//Exporting module info
//----------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetExportingModuleID() const
{
	return exportingModuleID;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetExportingModuleID(unsigned int aexportingModuleID)
{
	exportingModuleID = aexportingModuleID;
}

//----------------------------------------------------------------------------------------
const wchar_t* ConnectorInfo::GetConnectorClassNameInternal() const
{
	return systemClassName.c_str();
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetConnectorClassNameInternal(const wchar_t* aconnectorClassName)
{
	connectorClassName = aconnectorClassName;
}

//----------------------------------------------------------------------------------------
const wchar_t* ConnectorInfo::GetExportingModuleConnectorInstanceNameInternal() const
{
	return exportingModuleConnectorInstanceName.c_str();
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetExportingModuleConnectorInstanceNameInternal(const wchar_t* aexportingModuleConnectorInstanceName)
{
	exportingModuleConnectorInstanceName = aexportingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------
const wchar_t* ConnectorInfo::GetSystemClassNameInternal() const
{
	return systemClassName.c_str();
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetSystemClassNameInternal(const wchar_t* asystemClassName)
{
	systemClassName = asystemClassName;
}

//----------------------------------------------------------------------------------------
//Importing module info
//----------------------------------------------------------------------------------------
bool ConnectorInfo::GetIsConnectorUsed() const
{
	return connectorUsed;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetIsConnectorUsed(bool aconnectorUser)
{
	connectorUsed = aconnectorUser;
}

//----------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetImportingModuleID() const
{
	return importingModuleID;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetImportingModuleID(unsigned int aimportingModuleID)
{
	importingModuleID = aimportingModuleID;
}

//----------------------------------------------------------------------------------------
const wchar_t* ConnectorInfo::GetImportingModuleConnectorInstanceNameInternal() const
{
	return importingModuleConnectorInstanceName.c_str();
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetImportingModuleConnectorInstanceNameInternal(const wchar_t* aimportingModuleConnectorInstanceName)
{
	importingModuleConnectorInstanceName = aimportingModuleConnectorInstanceName;
}
