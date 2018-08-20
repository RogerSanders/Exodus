#include "ModuleSettingOptionInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ModuleSettingOptionInfo::ModuleSettingOptionInfo()
{ }

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ModuleSettingOptionInfo::GetIModuleSettingOptionInfoVersion() const
{
	return ThisIModuleSettingOptionInfoVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ModuleSettingOptionInfo::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ModuleSettingOptionInfo::GetDisplayName() const
{
	return _displayName;
}

//----------------------------------------------------------------------------------------------------------------------
// Setters
//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetName(const Marshal::In<std::wstring>& name)
{
	_name = name;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetDisplayName(const Marshal::In<std::wstring>& displayName)
{
	_displayName = displayName;
}
