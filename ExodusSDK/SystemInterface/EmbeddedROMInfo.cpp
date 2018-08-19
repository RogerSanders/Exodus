#include "EmbeddedROMInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
EmbeddedROMInfo::EmbeddedROMInfo()
:_targetDevice(0), _moduleID(0), _targetInterfaceNo(0), _romRegionSize(0), _romEntryBitCount(0)
{}

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetIEmbeddedROMInfoVersion() const
{
	return ThisIEmbeddedROMInfoVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> EmbeddedROMInfo::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> EmbeddedROMInfo::GetDisplayName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetModuleID() const
{
	return _moduleID;
}

//----------------------------------------------------------------------------------------------------------------------
IDevice* EmbeddedROMInfo::GetTargetDevice() const
{
	return _targetDevice;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetTargetInterfaceNo() const
{
	return _targetInterfaceNo;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetROMRegionSize() const
{
	return _romRegionSize;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetROMEntryBitCount() const
{
	return _romEntryBitCount;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> EmbeddedROMInfo::GetFilePath() const
{
	return _filePath;
}

//----------------------------------------------------------------------------------------------------------------------
// Setters
//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetName(const Marshal::In<std::wstring>& name)
{
	_name = name;
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetDisplayName(const Marshal::In<std::wstring>& displayName)
{
	_displayName = displayName;
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetModuleID(unsigned int moduleID)
{
	_moduleID = moduleID;
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetTargetDevice(IDevice* targetDevice)
{
	_targetDevice = targetDevice;
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetTargetInterfaceNo(unsigned int targetInterfaceNo)
{
	_targetInterfaceNo = targetInterfaceNo;
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetROMRegionSize(unsigned int romRegionSize)
{
	_romRegionSize = romRegionSize;
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetROMEntryBitCount(unsigned int romEntryBitCount)
{
	_romEntryBitCount = romEntryBitCount;
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetFilePath(const Marshal::In<std::wstring>& filePath)
{
	_filePath = filePath;
}
