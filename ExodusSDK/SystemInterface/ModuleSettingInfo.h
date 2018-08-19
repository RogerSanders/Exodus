#ifndef __MODULESETTINGINFO_H__
#define __MODULESETTINGINFO_H__
#include "IModuleSettingInfo.h"

class ModuleSettingInfo :public IModuleSettingInfo
{
public:
	// Constructors
	ModuleSettingInfo();

	// Interface version functions
	virtual unsigned int GetIModuleSettingInfoVersion() const;

	// Getters
	virtual Marshal::Ret<std::wstring> GetName() const;
	virtual Marshal::Ret<std::wstring> GetDisplayName() const;
	virtual unsigned int GetOptionCount() const;
	virtual unsigned int GetDefaultOptionIndex() const;
	virtual unsigned int GetToggleSettingOnOptionIndex() const;
	virtual unsigned int GetToggleSettingOffOptionIndex() const;
	virtual bool IsToggleSetting() const;

	// Setters
	virtual void SetName(const Marshal::In<std::wstring>& name);
	virtual void SetDisplayName(const Marshal::In<std::wstring>& displayName);
	virtual void SetOptionCount(unsigned int optionCount);
	virtual void SetDefaultOptionIndex(unsigned int defaultOptionIndex);
	virtual void SetToggleSettingOnOptionIndex(unsigned int onOptionIndex);
	virtual void SetToggleSettingOffOptionIndex(unsigned int offOptionIndex);
	virtual void SetIsToggleSetting(bool toggleSetting);

private:
	std::wstring _name;
	std::wstring _displayName;
	unsigned int _optionCount;
	unsigned int _defaultOptionIndex;
	unsigned int _onOptionIndex;
	unsigned int _offOptionIndex;
	bool _toggleSetting;
};

#endif
