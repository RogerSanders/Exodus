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
	return _allocator;
}

//----------------------------------------------------------------------------------------
DeviceInfo::DestructorPointer DeviceInfo::GetDestructor() const
{
	return _destructor;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> DeviceInfo::GetDeviceClassName() const
{
	return _className;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> DeviceInfo::GetDeviceImplementationName() const
{
	return _implementationName;
}

//----------------------------------------------------------------------------------------
unsigned int DeviceInfo::GetDeviceVersionNo() const
{
	return _versionNo;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> DeviceInfo::GetDeviceCopyright() const
{
	return _copyright;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> DeviceInfo::GetDeviceComments() const
{
	return _comments;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceSettings(AllocatorPointer allocator, DestructorPointer destructor, const Marshal::In<std::wstring>& deviceClassName, const Marshal::In<std::wstring>& deviceImplementationName, unsigned int deviceVersionNo, const Marshal::In<std::wstring>& deviceCopyright, const Marshal::In<std::wstring>& deviceComments)
{
	SetDeviceAllocators(allocator, destructor);
	SetDeviceClassName(deviceClassName);
	SetDeviceImplementationName(deviceImplementationName);
	SetDeviceVersionNo(deviceVersionNo);
	SetDeviceCopyright(deviceCopyright);
	SetDeviceComments(deviceComments);
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceAllocators(AllocatorPointer allocator, DestructorPointer destructor)
{
	_allocator = allocator;
	_destructor = destructor;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceClassName(const Marshal::In<std::wstring>& deviceClassName)
{
	_className = deviceClassName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceImplementationName(const Marshal::In<std::wstring>& deviceImplementationName)
{
	_implementationName = deviceImplementationName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceVersionNo(unsigned int deviceVersionNo)
{
	_versionNo = deviceVersionNo;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceCopyright(const Marshal::In<std::wstring>& deviceCopyright)
{
	_copyright = deviceCopyright;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceComments(const Marshal::In<std::wstring>& deviceComments)
{
	_comments = deviceComments;
}
