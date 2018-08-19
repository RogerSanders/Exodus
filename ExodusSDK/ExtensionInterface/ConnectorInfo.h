#ifndef __CONNECTORINFO_H__
#define __CONNECTORINFO_H__
#include "IConnectorInfo.h"

class ConnectorInfo :public IConnectorInfo
{
public:
	// Constructors
	ConnectorInfo();

	// Interface version functions
	virtual unsigned int GetIConnectorInfoVersion() const;

	// Internal data
	virtual unsigned int GetConnectorID() const;
	virtual void SetConnectorID(unsigned int connectorID);

	// Exporting module info
	virtual unsigned int GetExportingModuleID() const;
	virtual void SetExportingModuleID(unsigned int exportingModuleID);
	virtual Marshal::Ret<std::wstring> GetConnectorClassName() const;
	virtual void SetConnectorClassName(const Marshal::In<std::wstring>& connectorClassName);
	virtual Marshal::Ret<std::wstring> GetExportingModuleConnectorInstanceName() const;
	virtual void SetExportingModuleConnectorInstanceName(const Marshal::In<std::wstring>& exportingModuleConnectorInstanceName);
	virtual Marshal::Ret<std::wstring> GetSystemClassName() const;
	virtual void SetSystemClassName(const Marshal::In<std::wstring>& systemClassName);

	// Importing module info
	virtual bool GetIsConnectorUsed() const;
	virtual void SetIsConnectorUsed(bool connectorUser);
	virtual unsigned int GetImportingModuleID() const;
	virtual void SetImportingModuleID(unsigned int importingModuleID);
	virtual Marshal::Ret<std::wstring> GetImportingModuleConnectorInstanceName() const;
	virtual void SetImportingModuleConnectorInstanceName(const Marshal::In<std::wstring>& importingModuleConnectorInstanceName);

private:
	// Internal data
	unsigned int _connectorID;

	// Exporting module info
	unsigned int _exportingModuleID;
	std::wstring _connectorClassName;
	std::wstring _exportingModuleConnectorInstanceName;
	std::wstring _systemClassName;

	// Importing module info
	bool _connectorUsed;
	unsigned int _importingModuleID;
	std::wstring _importingModuleConnectorInstanceName;
};

#endif
