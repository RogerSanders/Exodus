#include "ConnectorInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ConnectorInfo::ConnectorInfo()
:_connectorID(0), _exportingModuleID(0), _connectorUsed(false), _importingModuleID(0)
{}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetIConnectorInfoVersion() const
{
	return ThisIConnectorInfoVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Internal data
//----------------------------------------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetConnectorID() const
{
	return _connectorID;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetConnectorID(unsigned int connectorID)
{
	_connectorID = connectorID;
}

//----------------------------------------------------------------------------------------------------------------------
// Exporting module info
//----------------------------------------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetExportingModuleID() const
{
	return _exportingModuleID;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetExportingModuleID(unsigned int exportingModuleID)
{
	_exportingModuleID = exportingModuleID;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ConnectorInfo::GetConnectorClassName() const
{
	return _connectorClassName;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetConnectorClassName(const Marshal::In<std::wstring>& connectorClassName)
{
	_connectorClassName = connectorClassName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ConnectorInfo::GetExportingModuleConnectorInstanceName() const
{
	return _exportingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetExportingModuleConnectorInstanceName(const Marshal::In<std::wstring>& exportingModuleConnectorInstanceName)
{
	_exportingModuleConnectorInstanceName = exportingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ConnectorInfo::GetSystemClassName() const
{
	return _systemClassName;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetSystemClassName(const Marshal::In<std::wstring>& systemClassName)
{
	_systemClassName = systemClassName;
}

//----------------------------------------------------------------------------------------------------------------------
// Importing module info
//----------------------------------------------------------------------------------------------------------------------
bool ConnectorInfo::GetIsConnectorUsed() const
{
	return _connectorUsed;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetIsConnectorUsed(bool connectorUser)
{
	_connectorUsed = connectorUser;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ConnectorInfo::GetImportingModuleID() const
{
	return _importingModuleID;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetImportingModuleID(unsigned int importingModuleID)
{
	_importingModuleID = importingModuleID;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ConnectorInfo::GetImportingModuleConnectorInstanceName() const
{
	return _importingModuleConnectorInstanceName;
}

//----------------------------------------------------------------------------------------------------------------------
void ConnectorInfo::SetImportingModuleConnectorInstanceName(const Marshal::In<std::wstring>& importingModuleConnectorInstanceName)
{
	_importingModuleConnectorInstanceName = importingModuleConnectorInstanceName;
}
