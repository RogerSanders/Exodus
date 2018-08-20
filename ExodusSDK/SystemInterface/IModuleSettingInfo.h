#ifndef __IMODULESETTINGINFO_H__
#define __IMODULESETTINGINFO_H__
#include <ExtensionInterface/ExtensionInterface.pkg>
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
using namespace MarshalSupport::Operators;

class IModuleSettingInfo
{
public:
	// Constructors
	inline virtual ~IModuleSettingInfo() = 0;

	// Interface version functions
	static inline unsigned int ThisIModuleSettingInfoVersion() { return 1; }
	virtual unsigned int GetIModuleSettingInfoVersion() const = 0;

	// Getters
	virtual Marshal::Ret<std::wstring> GetName() const = 0;
	virtual Marshal::Ret<std::wstring> GetDisplayName() const = 0;
	virtual unsigned int GetOptionCount() const = 0;
	virtual unsigned int GetDefaultOptionIndex() const = 0;
	virtual unsigned int GetToggleSettingOnOptionIndex() const = 0;
	virtual unsigned int GetToggleSettingOffOptionIndex() const = 0;
	virtual bool IsToggleSetting() const = 0;

	// Setters
	virtual void SetName(const Marshal::In<std::wstring>& name) = 0;
	virtual void SetDisplayName(const Marshal::In<std::wstring>& displayName) = 0;
	virtual void SetOptionCount(unsigned int optionCount) = 0;
	virtual void SetDefaultOptionIndex(unsigned int defaultOptionIndex) = 0;
	virtual void SetToggleSettingOnOptionIndex(unsigned int onOptionIndex) = 0;
	virtual void SetToggleSettingOffOptionIndex(unsigned int offOptionIndex) = 0;
	virtual void SetIsToggleSetting(bool toggleSetting) = 0;
};
IModuleSettingInfo::~IModuleSettingInfo() { }

#endif
