#ifndef __ICONNECTORINFO_H__
#define __ICONNECTORINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IConnectorInfo
{
public:
	//Constructors
	virtual ~IConnectorInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIConnectorInfoVersion(){ return 1; }
	virtual unsigned int GetIConnectorInfoVersion() const = 0;

	//Internal data
	virtual unsigned int GetConnectorID() const = 0;
	virtual void SetConnectorID(unsigned int aconnectorID) = 0;

	//Exporting module info
	virtual unsigned int GetExportingModuleID() const = 0;
	virtual void SetExportingModuleID(unsigned int aexportingModuleID) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetConnectorClassName() const = 0;
	virtual void SetConnectorClassName(const MarshalSupport::Marshal::In<std::wstring>& aconnectorClassName) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetExportingModuleConnectorInstanceName() const = 0;
	virtual void SetExportingModuleConnectorInstanceName(const MarshalSupport::Marshal::In<std::wstring>& aexportingModuleConnectorInstanceName) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemClassName() const = 0;
	virtual void SetSystemClassName(const MarshalSupport::Marshal::In<std::wstring>& asystemClassName) = 0;

	//Importing module info
	virtual bool GetIsConnectorUsed() const = 0;
	virtual void SetIsConnectorUsed(bool aconnectorUser) = 0;
	virtual unsigned int GetImportingModuleID() const = 0;
	virtual void SetImportingModuleID(unsigned int aimportingModuleID) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetImportingModuleConnectorInstanceName() const = 0;
	virtual void SetImportingModuleConnectorInstanceName(const MarshalSupport::Marshal::In<std::wstring>& aimportingModuleConnectorInstanceName) = 0;
};

#endif
