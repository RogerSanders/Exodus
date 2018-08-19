#include "LoadedModuleInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
LoadedModuleInfo::LoadedModuleInfo()
:_moduleID(0), _programModule(false)
{}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int LoadedModuleInfo::GetILoadedModuleInfoVersion() const
{
	return ThisILoadedModuleInfoVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Internal data
//----------------------------------------------------------------------------------------------------------------------
unsigned int LoadedModuleInfo::GetModuleID() const
{
	return _moduleID;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleID(unsigned int moduleID)
{
	_moduleID = moduleID;
}

//----------------------------------------------------------------------------------------------------------------------
// External information
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleFilePath() const
{
	return _filePath;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleFilePath(const Marshal::In<std::wstring>& moduleFilePath)
{
	_filePath = moduleFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
// Required metadata
//----------------------------------------------------------------------------------------------------------------------
bool LoadedModuleInfo::GetIsProgramModule() const
{
	return _programModule;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetIsProgramModule(bool programModule)
{
	_programModule = programModule;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetSystemClassName() const
{
	return _systemClassName;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetSystemClassName(const Marshal::In<std::wstring>& systemClassName)
{
	_systemClassName = systemClassName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleClassName() const
{
	return _className;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleClassName(const Marshal::In<std::wstring>& className)
{
	_className = className;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleInstanceName() const
{
	return _instanceName;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleInstanceName(const Marshal::In<std::wstring>& instanceName)
{
	_instanceName = instanceName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetModuleDisplayName() const
{
	return _displayName;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetModuleDisplayName(const Marshal::In<std::wstring>& displayName)
{
	_displayName = displayName;
}

//----------------------------------------------------------------------------------------------------------------------
// Optional metadata
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetProductionYear() const
{
	return _productionYear;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetProductionYear(const Marshal::In<std::wstring>& productionYear)
{
	_productionYear = productionYear;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetManufacturerCode() const
{
	return _manufacturerCode;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerCode(const Marshal::In<std::wstring>& manufacturerCode)
{
	_manufacturerCode = manufacturerCode;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> LoadedModuleInfo::GetManufacturerDisplayName() const
{
	return _manufacturerDisplayName;
}

//----------------------------------------------------------------------------------------------------------------------
void LoadedModuleInfo::SetManufacturerDisplayName(const Marshal::In<std::wstring>& manufacturerDisplayName)
{
	_manufacturerDisplayName = manufacturerDisplayName;
}
