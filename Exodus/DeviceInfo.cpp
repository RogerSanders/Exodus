#include "DeviceInfo.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int DeviceInfo::GetIDeviceInfoVersion() const
{
	return ThisIDeviceInfoVersion();
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceVersionNo(unsigned int adeviceVersionNo)
{
	deviceVersionNo = adeviceVersionNo;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor)
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceNameInternal(const wchar_t* adeviceName)
{
	deviceName = adeviceName;
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
unsigned int DeviceInfo::GetDeviceVersionNo() const
{
	return deviceVersionNo;
}

//----------------------------------------------------------------------------------------
std::wstring DeviceInfo::GetDeviceName() const
{
	return deviceName;
}

//----------------------------------------------------------------------------------------
DeviceInfo::AllocatorPointer DeviceInfo::GetAllocator() const
{
	return Allocator;
}

//----------------------------------------------------------------------------------------
DeviceInfo::DestructorPointer DeviceInfo::GetDestructor() const
{
	return Destructor;
}
