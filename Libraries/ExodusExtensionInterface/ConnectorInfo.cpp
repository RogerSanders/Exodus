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
void ConnectorInfo::GetConnectorClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetConnectorClassName());
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetConnectorClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	marshaller.MarshalTo(connectorClassName);
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::GetExportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetExportingModuleConnectorInstanceName());
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetExportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	marshaller.MarshalTo(exportingModuleConnectorInstanceName);
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::GetSystemClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetSystemClassName());
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetSystemClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	marshaller.MarshalTo(systemClassName);
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
void ConnectorInfo::GetImportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetImportingModuleConnectorInstanceName());
}

//----------------------------------------------------------------------------------------
void ConnectorInfo::SetImportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	marshaller.MarshalTo(importingModuleConnectorInstanceName);
}
