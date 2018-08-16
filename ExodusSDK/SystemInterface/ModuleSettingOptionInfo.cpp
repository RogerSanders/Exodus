#include "ModuleSettingOptionInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ModuleSettingOptionInfo::ModuleSettingOptionInfo()
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ModuleSettingOptionInfo::GetIModuleSettingOptionInfoVersion() const
{
	return ThisIModuleSettingOptionInfoVersion();
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ModuleSettingOptionInfo::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ModuleSettingOptionInfo::GetDisplayName() const
{
	return displayName;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetName(const Marshal::In<std::wstring>& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetDisplayName(const Marshal::In<std::wstring>& adisplayName)
{
	displayName = adisplayName;
}
