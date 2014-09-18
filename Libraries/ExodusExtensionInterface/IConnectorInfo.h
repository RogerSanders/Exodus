#ifndef __ICONNECTORINFO_H__
#define __ICONNECTORINFO_H__
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class IConnectorInfo
{
public:
	//Constructors
	virtual ~IConnectorInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIConnectorInfoVersion();
	virtual unsigned int GetIConnectorInfoVersion() const = 0;

	//Internal data
	virtual unsigned int GetConnectorID() const = 0;
	virtual void SetConnectorID(unsigned int aconnectorID) = 0;

	//Exporting module info
	virtual unsigned int GetExportingModuleID() const = 0;
	virtual void SetExportingModuleID(unsigned int aexportingModuleID) = 0;
	inline std::wstring GetConnectorClassName() const;
	inline void SetConnectorClassName(const std::wstring& aconnectorClassName);
	inline std::wstring GetExportingModuleConnectorInstanceName() const;
	inline void SetExportingModuleConnectorInstanceName(const std::wstring& aexportingModuleConnectorInstanceName);
	inline std::wstring GetSystemClassName() const;
	inline void SetSystemClassName(const std::wstring& asystemClassName);

	//Importing module info
	virtual bool GetIsConnectorUsed() const = 0;
	virtual void SetIsConnectorUsed(bool aconnectorUser) = 0;
	virtual unsigned int GetImportingModuleID() const = 0;
	virtual void SetImportingModuleID(unsigned int aimportingModuleID) = 0;
	inline std::wstring GetImportingModuleConnectorInstanceName() const;
	inline void SetImportingModuleConnectorInstanceName(const std::wstring& aimportingModuleConnectorInstanceName);

protected:
	//Exporting module info
	virtual void GetConnectorClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetConnectorClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetExportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetExportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetSystemClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetSystemClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;

	//Importing module info
	virtual void GetImportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetImportingModuleConnectorInstanceNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
};

#include "IConnectorInfo.inl"
#endif
