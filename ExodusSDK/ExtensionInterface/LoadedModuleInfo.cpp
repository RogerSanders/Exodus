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
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleFilePath() const
{
	return filePath;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleFilePath(const Marshal::In<std::wstring>& amoduleFilePath)
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
Marshal::Ret<std::wstring> LoadedModuleInfo::GetSystemClassName() const
{
	return systemClassName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetSystemClassName(const Marshal::In<std::wstring>& asystemClassName)
{
	systemClassName = asystemClassName;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleClassName(const Marshal::In<std::wstring>& aclassName)
{
	className = aclassName;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleInstanceName() const
{
	return instanceName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleInstanceName(const Marshal::In<std::wstring>& ainstanceName)
{
	instanceName = ainstanceName;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleDisplayName() const
{
	return displayName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleDisplayName(const Marshal::In<std::wstring>& adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
//Optional metadata
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetProductionYear() const
{
	return productionYear;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetProductionYear(const Marshal::In<std::wstring>& aproductionYear)
{
	productionYear = aproductionYear;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetManufacturerCode() const
{
	return manufacturerCode;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerCode(const Marshal::In<std::wstring>& amanufacturerCode)
{
	manufacturerCode = amanufacturerCode;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetManufacturerDisplayName() const
{
	return manufacturerDisplayName;
}

//----------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerDisplayName(const Marshal::In<std::wstring>& amanufacturerDisplayName)
{
	manufacturerDisplayName = amanufacturerDisplayName;
}
