#ifndef __MODULESETTINGINFO_H__
#define __MODULESETTINGINFO_H__
#include "IModuleSettingInfo.h"

class ModuleSettingInfo :public IModuleSettingInfo
{
public:
	//Constructors
	ModuleSettingInfo();

	//Interface version functions
	virtual unsigned int GetIModuleSettingInfoVersion() const;

	//Getters
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDisplayName() const;
	virtual unsigned int GetOptionCount() const;
	virtual unsigned int GetDefaultOptionIndex() const;
	virtual unsigned int GetToggleSettingOnOptionIndex() const;
	virtual unsigned int GetToggleSettingOffOptionIndex() const;
	virtual bool IsToggleSetting() const;

	//Setters
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& aname);
	virtual void SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName);
	virtual void SetOptionCount(unsigned int aoptionCount);
	virtual void SetDefaultOptionIndex(unsigned int adefaultOptionIndex);
	virtual void SetToggleSettingOnOptionIndex(unsigned int aonOptionIndex);
	virtual void SetToggleSettingOffOptionIndex(unsigned int aoffOptionIndex);
	virtual void SetIsToggleSetting(bool atoggleSetting);

private:
	std::wstring name;
	std::wstring displayName;
	unsigned int optionCount;
	unsigned int defaultOptionIndex;
	unsigned int onOptionIndex;
	unsigned int offOptionIndex;
	bool toggleSetting;
};

#endif
