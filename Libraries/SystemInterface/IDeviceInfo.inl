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
	SetDeviceClassNameInternal(InteropSupport::STLObjectSource<std::wstring>(adeviceClassName));
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceImplementationName(const std::wstring& adeviceImplementationName)
{
	SetDeviceImplementationNameInternal(InteropSupport::STLObjectSource<std::wstring>(adeviceImplementationName));
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceCopyright(const std::wstring& adeviceCopyright)
{
	SetDeviceCopyrightInternal(InteropSupport::STLObjectSource<std::wstring>(adeviceCopyright));
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceComments(const std::wstring& adeviceComments)
{
	SetDeviceCommentsInternal(InteropSupport::STLObjectSource<std::wstring>(adeviceComments));
}
