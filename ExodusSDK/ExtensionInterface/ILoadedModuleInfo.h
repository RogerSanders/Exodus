#ifndef __ILOADEDMODULEINFO_H__
#define __ILOADEDMODULEINFO_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
using namespace MarshalSupport::Operators;

class ILoadedModuleInfo
{
public:
	// Constructors
	inline virtual ~ILoadedModuleInfo() = 0;

	// Interface version functions
	static inline unsigned int ThisILoadedModuleInfoVersion() { return 1; }
	virtual unsigned int GetILoadedModuleInfoVersion() const = 0;

	// Internal data
	virtual unsigned int GetModuleID() const = 0;
	virtual void SetModuleID(unsigned int moduleID) = 0;

	// External information
	virtual Marshal::Ret<std::wstring> GetModuleFilePath() const = 0;
	virtual void SetModuleFilePath(const Marshal::In<std::wstring>& moduleFilePath) = 0;

	// Required metadata
	virtual bool GetIsProgramModule() const = 0;
	virtual void SetIsProgramModule(bool programModule) = 0;
	virtual Marshal::Ret<std::wstring> GetSystemClassName() const = 0;
	virtual void SetSystemClassName(const Marshal::In<std::wstring>& systemClassName) = 0;
	virtual Marshal::Ret<std::wstring> GetModuleClassName() const = 0;
	virtual void SetModuleClassName(const Marshal::In<std::wstring>& className) = 0;
	virtual Marshal::Ret<std::wstring> GetModuleInstanceName() const = 0;
	virtual void SetModuleInstanceName(const Marshal::In<std::wstring>& instanceName) = 0;
	virtual Marshal::Ret<std::wstring> GetModuleDisplayName() const = 0;
	virtual void SetModuleDisplayName(const Marshal::In<std::wstring>& displayName) = 0;

	// Optional metadata
	virtual Marshal::Ret<std::wstring> GetProductionYear() const = 0;
	virtual void SetProductionYear(const Marshal::In<std::wstring>& productionYear) = 0;
	virtual Marshal::Ret<std::wstring> GetManufacturerCode() const = 0;
	virtual void SetManufacturerCode(const Marshal::In<std::wstring>& manufacturerCode) = 0;
	virtual Marshal::Ret<std::wstring> GetManufacturerDisplayName() const = 0;
	virtual void SetManufacturerDisplayName(const Marshal::In<std::wstring>& manufacturerDisplayName) = 0;
};
ILoadedModuleInfo::~ILoadedModuleInfo() { }

#endif
