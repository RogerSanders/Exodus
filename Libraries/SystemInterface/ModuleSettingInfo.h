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
	std::wstring GetName() const;
	std::wstring GetDisplayName() const;
	virtual unsigned int GetOptionCount() const;
	virtual unsigned int GetDefaultOptionIndex() const;
	virtual unsigned int GetToggleSettingOnOptionIndex() const;
	virtual unsigned int GetToggleSettingOffOptionIndex() const;
	virtual bool IsToggleSetting() const;

	//Setters
	void SetName(const std::wstring& aname);
	void SetDisplayName(const std::wstring& adisplayName);
	virtual void SetOptionCount(unsigned int aoptionCount);
	virtual void SetDefaultOptionIndex(unsigned int adefaultOptionIndex);
	virtual void SetToggleSettingOnOptionIndex(unsigned int aonOptionIndex);
	virtual void SetToggleSettingOffOptionIndex(unsigned int aoffOptionIndex);
	virtual void SetIsToggleSetting(bool atoggleSetting);

protected:
	//Getters
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Setters
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller);
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller);

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
