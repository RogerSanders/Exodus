#include "DeviceInfo.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int DeviceInfo::GetIDeviceInfoVersion() const
{
	return ThisIDeviceInfoVersion();
}

//----------------------------------------------------------------------------------------
//Getters
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

//----------------------------------------------------------------------------------------
std::wstring DeviceInfo::GetDeviceClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
std::wstring DeviceInfo::GetDeviceImplementationName() const
{
	return implementationName;
}

//----------------------------------------------------------------------------------------
unsigned int DeviceInfo::GetDeviceVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
std::wstring DeviceInfo::GetDeviceCopyright() const
{
	return copyright;
}

//----------------------------------------------------------------------------------------
std::wstring DeviceInfo::GetDeviceComments() const
{
	return comments;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor)
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceClassNameInternal(const wchar_t* adeviceClassName)
{
	className = adeviceClassName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceImplementationNameInternal(const wchar_t* adeviceImplementationName)
{
	implementationName = adeviceImplementationName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceVersionNo(unsigned int adeviceVersionNo)
{
	versionNo = adeviceVersionNo;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceCopyrightInternal(const wchar_t* adeviceCopyright)
{
	copyright = adeviceCopyright;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceCommentsInternal(const wchar_t* adeviceComments)
{
	comments = adeviceComments;
}
