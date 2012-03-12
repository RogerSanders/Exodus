#include "DeviceInfo.h"

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceVersionNo(unsigned int adeviceVersionNo)
{
	deviceVersionNo = adeviceVersionNo;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceAllocators(IDevice* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IDevice*))
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
