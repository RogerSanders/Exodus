#include "ModuleSettingInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ModuleSettingInfo::ModuleSettingInfo()
:optionCount(0), defaultOptionIndex(0), onOptionIndex(0), offOptionIndex(0), toggleSetting(false)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetIModuleSettingInfoVersion() const
{
	return ThisIModuleSettingInfoVersion();
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ModuleSettingInfo::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ModuleSettingInfo::GetDisplayName() const
{
	return displayName;
}

//----------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetOptionCount() const
{
	return optionCount;
}

//----------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetDefaultOptionIndex() const
{
	return defaultOptionIndex;
}

//----------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetToggleSettingOnOptionIndex() const
{
	return onOptionIndex;
}

//----------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetToggleSettingOffOptionIndex() const
{
	return offOptionIndex;
}

//----------------------------------------------------------------------------------------
bool ModuleSettingInfo::IsToggleSetting() const
{
	return toggleSetting;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetName(const MarshalSupport::Marshal::In<std::wstring>& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetOptionCount(unsigned int aoptionCount)
{
	optionCount = aoptionCount;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetDefaultOptionIndex(unsigned int adefaultOptionIndex)
{
	defaultOptionIndex = adefaultOptionIndex;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetToggleSettingOnOptionIndex(unsigned int aonOptionIndex)
{
	onOptionIndex = aonOptionIndex;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetToggleSettingOffOptionIndex(unsigned int aoffOptionIndex)
{
	offOptionIndex = aoffOptionIndex;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetIsToggleSetting(bool atoggleSetting)
{
	toggleSetting = atoggleSetting;
}
