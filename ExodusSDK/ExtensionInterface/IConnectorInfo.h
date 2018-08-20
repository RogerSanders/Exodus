#ifndef __ICONNECTORINFO_H__
#define __ICONNECTORINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
using namespace MarshalSupport::Operators;

class IConnectorInfo
{
public:
	// Constructors
	virtual ~IConnectorInfo() = 0 { }

	// Interface version functions
	static inline unsigned int ThisIConnectorInfoVersion(){ return 1; }
	virtual unsigned int GetIConnectorInfoVersion() const = 0;

	// Internal data
	virtual unsigned int GetConnectorID() const = 0;
	virtual void SetConnectorID(unsigned int connectorID) = 0;

	// Exporting module info
	virtual unsigned int GetExportingModuleID() const = 0;
	virtual void SetExportingModuleID(unsigned int exportingModuleID) = 0;
	virtual Marshal::Ret<std::wstring> GetConnectorClassName() const = 0;
	virtual void SetConnectorClassName(const Marshal::In<std::wstring>& connectorClassName) = 0;
	virtual Marshal::Ret<std::wstring> GetExportingModuleConnectorInstanceName() const = 0;
	virtual void SetExportingModuleConnectorInstanceName(const Marshal::In<std::wstring>& exportingModuleConnectorInstanceName) = 0;
	virtual Marshal::Ret<std::wstring> GetSystemClassName() const = 0;
	virtual void SetSystemClassName(const Marshal::In<std::wstring>& systemClassName) = 0;

	// Importing module info
	virtual bool GetIsConnectorUsed() const = 0;
	virtual void SetIsConnectorUsed(bool connectorUser) = 0;
	virtual unsigned int GetImportingModuleID() const = 0;
	virtual void SetImportingModuleID(unsigned int importingModuleID) = 0;
	virtual Marshal::Ret<std::wstring> GetImportingModuleConnectorInstanceName() const = 0;
	virtual void SetImportingModuleConnectorInstanceName(const Marshal::In<std::wstring>& importingModuleConnectorInstanceName) = 0;
};

#endif
