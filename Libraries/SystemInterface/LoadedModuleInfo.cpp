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
const wchar_t* LoadedModuleInfo::GetModuleFilePathInternal() const
{
	return filePath.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleFilePathInternal(const wchar_t* amoduleFilePath)
{
	filePath = amoduleFilePath;
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
const wchar_t* LoadedModuleInfo::GetSystemClassNameInternal() const
{
	return systemClassName.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetSystemClassNameInternal(const wchar_t* asystemClassName)
{
	systemClassName = asystemClassName;
}

//----------------------------------------------------------------------------------------
const wchar_t* LoadedModuleInfo::GetClassNameInternal() const
{
	return className.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetClassNameInternal(const wchar_t* aclassName)
{
	className = aclassName;
}

//----------------------------------------------------------------------------------------
const wchar_t* LoadedModuleInfo::GetInstanceNameInternal() const
{
	return instanceName.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetInstanceNameInternal(const wchar_t* ainstanceName)
{
	instanceName = ainstanceName;
}

//----------------------------------------------------------------------------------------
const wchar_t* LoadedModuleInfo::GetDisplayNameInternal() const
{
	return displayName.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetDisplayNameInternal(const wchar_t* adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
//Optional metadata
//----------------------------------------------------------------------------------------
const wchar_t* LoadedModuleInfo::GetProductionYearInternal() const
{
	return productionYear.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetProductionYearInternal(const wchar_t* aproductionYear)
{
	productionYear = aproductionYear;
}

//----------------------------------------------------------------------------------------
const wchar_t* LoadedModuleInfo::GetManufacturerCodeInternal() const
{
	return manufacturerCode.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerCodeInternal(const wchar_t* amanufacturerCode)
{
	manufacturerCode = amanufacturerCode;
}

//----------------------------------------------------------------------------------------
const wchar_t* LoadedModuleInfo::GetManufacturerDisplayNameInternal() const
{
	return manufacturerDisplayName.c_str();
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerDisplayNameInternal(const wchar_t* amanufacturerDisplayName)
{
	manufacturerDisplayName = amanufacturerDisplayName;
}
