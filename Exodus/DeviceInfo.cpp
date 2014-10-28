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
MarshalSupport::Marshal::Ret<std::wstring> DeviceInfo::GetDeviceClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> DeviceInfo::GetDeviceImplementationName() const
{
	return implementationName;
}

//----------------------------------------------------------------------------------------
unsigned int DeviceInfo::GetDeviceVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> DeviceInfo::GetDeviceCopyright() const
{
	return copyright;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> DeviceInfo::GetDeviceComments() const
{
	return comments;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const MarshalSupport::Marshal::In<std::wstring>& adeviceClassName, const MarshalSupport::Marshal::In<std::wstring>& adeviceImplementationName, unsigned int adeviceVersionNo, const MarshalSupport::Marshal::In<std::wstring>& adeviceCopyright, const MarshalSupport::Marshal::In<std::wstring>& adeviceComments)
{
	SetDeviceAllocators(aAllocator, aDestructor);
	SetDeviceClassName(adeviceClassName);
	SetDeviceImplementationName(adeviceImplementationName);
	SetDeviceVersionNo(adeviceVersionNo);
	SetDeviceCopyright(adeviceCopyright);
	SetDeviceComments(adeviceComments);
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor)
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceClassName(const MarshalSupport::Marshal::In<std::wstring>& adeviceClassName)
{
	className = adeviceClassName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceImplementationName(const MarshalSupport::Marshal::In<std::wstring>& adeviceImplementationName)
{
	implementationName = adeviceImplementationName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceVersionNo(unsigned int adeviceVersionNo)
{
	versionNo = adeviceVersionNo;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceCopyright(const MarshalSupport::Marshal::In<std::wstring>& adeviceCopyright)
{
	copyright = adeviceCopyright;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceComments(const MarshalSupport::Marshal::In<std::wstring>& adeviceComments)
{
	comments = adeviceComments;
}
