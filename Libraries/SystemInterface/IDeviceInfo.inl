//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IDeviceInfo::ThisIDeviceInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& adeviceClassName, const std::wstring& adeviceImplementationName, unsigned int adeviceVersionNo, const std::wstring& adeviceCopyright, const std::wstring& adeviceComments)
{
	SetDeviceAllocators(aAllocator, aDestructor);
	SetDeviceClassName(adeviceClassName);
	SetDeviceImplementationName(adeviceImplementationName);
	SetDeviceVersionNo(adeviceVersionNo);
	SetDeviceCopyright(adeviceCopyright);
	SetDeviceComments(adeviceComments);
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceClassName(const std::wstring& adeviceClassName)
{
	SetDeviceClassNameInternal(adeviceClassName.c_str());
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceImplementationName(const std::wstring& adeviceImplementationName)
{
	SetDeviceImplementationNameInternal(adeviceImplementationName.c_str());
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceCopyright(const std::wstring& adeviceCopyright)
{
	SetDeviceCopyrightInternal(adeviceCopyright.c_str());
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceComments(const std::wstring& adeviceComments)
{
	SetDeviceCommentsInternal(adeviceComments.c_str());
}
