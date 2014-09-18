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
std::wstring ModuleSettingOptionInfo::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
std::wstring ModuleSettingOptionInfo::GetDisplayName() const
{
	return displayName;
}

//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDisplayName());
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	SetName(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetDisplayName(const std::wstring& adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
void ModuleSettingOptionInfo::SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller)
{
	SetDisplayName(displayNameMarshaller.MarshalTo());
}
