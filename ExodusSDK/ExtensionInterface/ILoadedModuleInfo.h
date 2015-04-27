#ifndef __ILOADEDMODULEINFO_H__
#define __ILOADEDMODULEINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class ILoadedModuleInfo
{
public:
	//Constructors
	virtual ~ILoadedModuleInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisILoadedModuleInfoVersion() { return 1; }
	virtual unsigned int GetILoadedModuleInfoVersion() const = 0;

	//Internal data
	virtual unsigned int GetModuleID() const = 0;
	virtual void SetModuleID(unsigned int amoduleID) = 0;

	//External information
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleFilePath() const = 0;
	virtual void SetModuleFilePath(const MarshalSupport::Marshal::In<std::wstring>& amoduleFilePath) = 0;

	//Required metadata
	virtual bool GetIsProgramModule() const = 0;
	virtual void SetIsProgramModule(bool aprogramModule) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemClassName() const = 0;
	virtual void SetSystemClassName(const MarshalSupport::Marshal::In<std::wstring>& asystemClassName) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleClassName() const = 0;
	virtual void SetModuleClassName(const MarshalSupport::Marshal::In<std::wstring>& aclassName) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleInstanceName() const = 0;
	virtual void SetModuleInstanceName(const MarshalSupport::Marshal::In<std::wstring>& ainstanceName) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleDisplayName() const = 0;
	virtual void SetModuleDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName) = 0;

	//Optional metadata
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetProductionYear() const = 0;
	virtual void SetProductionYear(const MarshalSupport::Marshal::In<std::wstring>& aproductionYear) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetManufacturerCode() const = 0;
	virtual void SetManufacturerCode(const MarshalSupport::Marshal::In<std::wstring>& amanufacturerCode) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetManufacturerDisplayName() const = 0;
	virtual void SetManufacturerDisplayName(const MarshalSupport::Marshal::In<std::wstring>& amanufacturerDisplayName) = 0;
};

#endif
