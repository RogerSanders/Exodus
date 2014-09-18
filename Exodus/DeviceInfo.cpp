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
void DeviceInfo::GetDeviceClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDeviceClassName());
}

//----------------------------------------------------------------------------------------
void DeviceInfo::GetDeviceImplementationNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDeviceImplementationName());
}

//----------------------------------------------------------------------------------------
unsigned int DeviceInfo::GetDeviceVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::GetDeviceCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDeviceCopyright());
}

//----------------------------------------------------------------------------------------
void DeviceInfo::GetDeviceCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDeviceComments());
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
void DeviceInfo::SetDeviceClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetDeviceClassName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceImplementationNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetDeviceImplementationName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceVersionNo(unsigned int adeviceVersionNo)
{
	versionNo = adeviceVersionNo;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetDeviceCopyright(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetDeviceComments(marshaller.MarshalTo());
}
