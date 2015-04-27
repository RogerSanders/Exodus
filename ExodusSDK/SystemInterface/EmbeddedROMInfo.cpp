#include "EmbeddedROMInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EmbeddedROMInfo::EmbeddedROMInfo()
:targetDevice(0), moduleID(0), targetInterfaceNo(0), romRegionSize(0), romEntryBitCount(0)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetIEmbeddedROMInfoVersion() const
{
	return ThisIEmbeddedROMInfoVersion();
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> EmbeddedROMInfo::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> EmbeddedROMInfo::GetDisplayName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetModuleID() const
{
	return moduleID;
}

//----------------------------------------------------------------------------------------
IDevice* EmbeddedROMInfo::GetTargetDevice() const
{
	return targetDevice;
}

//----------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetTargetInterfaceNo() const
{
	return targetInterfaceNo;
}

//----------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetROMRegionSize() const
{
	return romRegionSize;
}

//----------------------------------------------------------------------------------------
unsigned int EmbeddedROMInfo::GetROMEntryBitCount() const
{
	return romEntryBitCount;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> EmbeddedROMInfo::GetFilePath() const
{
	return filePath;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetName(const MarshalSupport::Marshal::In<std::wstring>& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetModuleID(unsigned int amoduleID)
{
	moduleID = amoduleID;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetTargetDevice(IDevice* atargetDevice)
{
	targetDevice = atargetDevice;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetTargetInterfaceNo(unsigned int atargetInterfaceNo)
{
	targetInterfaceNo = atargetInterfaceNo;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetROMRegionSize(unsigned int aromRegionSize)
{
	romRegionSize = aromRegionSize;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetROMEntryBitCount(unsigned int aromEntryBitCount)
{
	romEntryBitCount = aromEntryBitCount;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetFilePath(const MarshalSupport::Marshal::In<std::wstring>& afilePath)
{
	filePath = afilePath;
}
