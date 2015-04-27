#ifndef __CONNECTORINFO_H__
#define __CONNECTORINFO_H__
#include "IConnectorInfo.h"

class ConnectorInfo :public IConnectorInfo
{
public:
	//Constructors
	ConnectorInfo();

	//Interface version functions
	virtual unsigned int GetIConnectorInfoVersion() const;

	//Internal data
	virtual unsigned int GetConnectorID() const;
	virtual void SetConnectorID(unsigned int aconnectorID);

	//Exporting module info
	virtual unsigned int GetExportingModuleID() const;
	virtual void SetExportingModuleID(unsigned int aexportingModuleID);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetConnectorClassName() const;
	virtual void SetConnectorClassName(const MarshalSupport::Marshal::In<std::wstring>& aconnectorClassName);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExportingModuleConnectorInstanceName() const;
	virtual void SetExportingModuleConnectorInstanceName(const MarshalSupport::Marshal::In<std::wstring>& aexportingModuleConnectorInstanceName);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemClassName() const;
	virtual void SetSystemClassName(const MarshalSupport::Marshal::In<std::wstring>& asystemClassName);

	//Importing module info
	virtual bool GetIsConnectorUsed() const;
	virtual void SetIsConnectorUsed(bool aconnectorUser);
	virtual unsigned int GetImportingModuleID() const;
	virtual void SetImportingModuleID(unsigned int aimportingModuleID);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetImportingModuleConnectorInstanceName() const;
	virtual void SetImportingModuleConnectorInstanceName(const MarshalSupport::Marshal::In<std::wstring>& aimportingModuleConnectorInstanceName);

private:
	//Internal data
	unsigned int connectorID;

	//Exporting module info
	unsigned int exportingModuleID;
	std::wstring connectorClassName;
	std::wstring exportingModuleConnectorInstanceName;
	std::wstring systemClassName;

	//Importing module info
	bool connectorUsed;
	unsigned int importingModuleID;
	std::wstring importingModuleConnectorInstanceName;
};

#endif
