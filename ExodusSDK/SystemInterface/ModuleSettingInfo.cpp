#include "ModuleSettingInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ModuleSettingInfo::ModuleSettingInfo()
:_optionCount(0), _defaultOptionIndex(0), _onOptionIndex(0), _offOptionIndex(0), _toggleSetting(false)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetIModuleSettingInfoVersion() const
{
	return ThisIModuleSettingInfoVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ModuleSettingInfo::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ModuleSettingInfo::GetDisplayName() const
{
	return _displayName;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetOptionCount() const
{
	return _optionCount;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetDefaultOptionIndex() const
{
	return _defaultOptionIndex;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetToggleSettingOnOptionIndex() const
{
	return _onOptionIndex;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ModuleSettingInfo::GetToggleSettingOffOptionIndex() const
{
	return _offOptionIndex;
}

//----------------------------------------------------------------------------------------------------------------------
bool ModuleSettingInfo::IsToggleSetting() const
{
	return _toggleSetting;
}

//----------------------------------------------------------------------------------------------------------------------
// Setters
//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingInfo::SetName(const Marshal::In<std::wstring>& name)
{
	_name = name;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingInfo::SetDisplayName(const Marshal::In<std::wstring>& displayName)
{
	_displayName = displayName;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingInfo::SetOptionCount(unsigned int optionCount)
{
	_optionCount = optionCount;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingInfo::SetDefaultOptionIndex(unsigned int defaultOptionIndex)
{
	_defaultOptionIndex = defaultOptionIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingInfo::SetToggleSettingOnOptionIndex(unsigned int onOptionIndex)
{
	_onOptionIndex = onOptionIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingInfo::SetToggleSettingOffOptionIndex(unsigned int offOptionIndex)
{
	_offOptionIndex = offOptionIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingInfo::SetIsToggleSetting(bool toggleSetting)
{
	_toggleSetting = toggleSetting;
}
