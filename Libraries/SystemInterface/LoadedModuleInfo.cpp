#include "LoadedModuleInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoadedModuleInfo::LoadedModuleInfo()
:moduleID(0), programModule(false)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int LoadedModuleInfo::GetILoadedModuleInfoVersion() const
{
	return ThisILoadedModuleInfoVersion();
}

//----------------------------------------------------------------------------------------
//Internal data
//----------------------------------------------------------------------------------------
unsigned int LoadedModuleInfo::GetModuleID() const
{
	return moduleID;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleID(unsigned int amoduleID)
{
	moduleID = amoduleID;
}

//----------------------------------------------------------------------------------------
//External information
//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetModuleFilePathInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetModuleFilePath());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetModuleFilePath(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
//Required metadata
//----------------------------------------------------------------------------------------
bool LoadedModuleInfo::GetIsProgramModule() const
{
	return programModule;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetIsProgramModule(bool aprogramModule)
{
	programModule = aprogramModule;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetSystemClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetSystemClassName());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetSystemClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetSystemClassName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetClassName());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetClassName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDisplayName());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetInstanceNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetInstanceName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDisplayName());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetDisplayName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
//Optional metadata
//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetProductionYearInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetProductionYear());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetProductionYearInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetProductionYear(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetManufacturerCodeInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetManufacturerCode());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerCodeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetManufacturerCode(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::GetManufacturerDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetManufacturerDisplayName());
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetManufacturerDisplayName(marshaller.MarshalTo());
}
