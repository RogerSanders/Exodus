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
std::wstring ModuleSettingInfo::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
std::wstring ModuleSettingInfo::GetDisplayName() const
{
	return displayName;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDisplayName());
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
void ModuleSettingInfo::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	SetName(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetDisplayName(const std::wstring& adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
void ModuleSettingInfo::SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller)
{
	SetDisplayName(displayNameMarshaller.MarshalTo());
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
