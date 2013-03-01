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
	inline std::wstring GetConnectorClassName() const;
	inline void SetConnectorClassName(const std::wstring& aconnectorClassName);
	inline std::wstring GetExportingModuleConnectorInstanceName() const;
	inline void SetExportingModuleConnectorInstanceName(const std::wstring& aexportingModuleConnectorInstanceName);
	inline std::wstring GetSystemClassName() const;
	inline void SetSystemClassName(const std::wstring& asystemClassName);

	//Importing module info
	virtual bool GetIsConnectorUsed() const;
	virtual void SetIsConnectorUsed(bool aconnectorUser);
	virtual unsigned int GetImportingModuleID() const;
	virtual void SetImportingModuleID(unsigned int aimportingModuleID);
	inline std::wstring GetImportingModuleConnectorInstanceName() const;
	inline void SetImportingModuleConnectorInstanceName(const std::wstring& aimportingModuleConnectorInstanceName);

protected:
	//Exporting module info
	virtual const wchar_t* GetConnectorClassNameInternal() const;
	virtual void SetConnectorClassNameInternal(const wchar_t* aconnectorClassName);
	virtual const wchar_t* GetExportingModuleConnectorInstanceNameInternal() const;
	virtual void SetExportingModuleConnectorInstanceNameInternal(const wchar_t* aexportingModuleConnectorInstanceName);
	virtual const wchar_t* GetSystemClassNameInternal() const;
	virtual void SetSystemClassNameInternal(const wchar_t* asystemClassName);

	//Importing module info
	virtual const wchar_t* GetImportingModuleConnectorInstanceNameInternal() const;
	virtual void SetImportingModuleConnectorInstanceNameInternal(const wchar_t* aimportingModuleConnectorInstanceName);

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

#include "ConnectorInfo.inl"
#endif
