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
std::wstring EmbeddedROMInfo::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
std::wstring EmbeddedROMInfo::GetDisplayName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDisplayName());
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
std::wstring EmbeddedROMInfo::GetFilePath() const
{
	return filePath;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::GetFilePathInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetFilePath());
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetName(const std::wstring& aname)
{
	name = aname;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	SetName(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetDisplayName(const std::wstring& adisplayName)
{
	displayName = adisplayName;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller)
{
	SetDisplayName(displayNameMarshaller.MarshalTo());
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
void EmbeddedROMInfo::SetFilePath(const std::wstring& afilePath)
{
	filePath = afilePath;
}

//----------------------------------------------------------------------------------------
void EmbeddedROMInfo::SetFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller)
{
	SetFilePath(filePathMarshaller.MarshalTo());
}
