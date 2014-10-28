#include "ConnectorInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ConnectorInfo::ConnectorInfo()
:connectorID(0), exportingModuleID(0), connectorUsed(false), importingModuleID(0)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetIConnectorInfoVersion() const
{
	return ThisIConnectorInfoVersion();
}

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
MarshalSupport::Marshal::Ret<std::wstring> ConnectorInfo::GetConnectorClassName() const
{
	return connectorClassName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetConnectorClassName(const MarshalSupport::Marshal::In<std::wstring>& aconnectorClassName)
{
	connectorClassName = aconnectorClassName;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ConnectorInfo::GetExportingModuleConnectorInstanceName() const
{
	return exportingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetExportingModuleConnectorInstanceName(const MarshalSupport::Marshal::In<std::wstring>& aexportingModuleConnectorInstanceName)
{
	exportingModuleConnectorInstanceName = aexportingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ConnectorInfo::GetSystemClassName() const
{
	return systemClassName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetSystemClassName(const MarshalSupport::Marshal::In<std::wstring>& asystemClassName)
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
MarshalSupport::Marshal::Ret<std::wstring> ConnectorInfo::GetImportingModuleConnectorInstanceName() const
{
	return importingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetImportingModuleConnectorInstanceName(const MarshalSupport::Marshal::In<std::wstring>& aimportingModuleConnectorInstanceName)
{
	importingModuleConnectorInstanceName = aimportingModuleConnectorInstanceName;
}
