//----------------------------------------------------------------------------------------
//Getters
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
void DeviceInfo::SetDeviceSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& adeviceClassName, const std::wstring& adeviceImplementationName, unsigned int adeviceVersionNo, const std::wstring& adeviceCopyright, const std::wstring& adeviceComments)
{
	SetDeviceAllocators(aAllocator, aDestructor);
	SetDeviceClassName(adeviceClassName);
	SetDeviceImplementationName(adeviceImplementationName);
	SetDeviceVersionNo(adeviceVersionNo);
	SetDeviceCopyright(adeviceCopyright);
	SetDeviceComments(adeviceComments);
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceClassName(const std::wstring& adeviceClassName)
{
	className = adeviceClassName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceImplementationName(const std::wstring& adeviceImplementationName)
{
	implementationName = adeviceImplementationName;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceCopyright(const std::wstring& adeviceCopyright)
{
	copyright = adeviceCopyright;
}

//----------------------------------------------------------------------------------------
void DeviceInfo::SetDeviceComments(const std::wstring& adeviceComments)
{
	comments = adeviceComments;
}
