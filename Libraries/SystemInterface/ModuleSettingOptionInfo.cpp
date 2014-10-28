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
MarshalSupport::Marshal::Ret<std::wstring> ModuleSettingOptionInfo::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ModuleSettingOptionInfo::GetDisplayName() const
{
	return displayName;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetName(const MarshalSupport::Marshal::In<std::wstring>& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName)
{
	displayName = adisplayName;
}
