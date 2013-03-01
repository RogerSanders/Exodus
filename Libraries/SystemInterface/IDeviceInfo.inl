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
void IDeviceInfo::SetDeviceSettings(const std::wstring& adeviceName, AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int adeviceVersionNo)
{
	SetDeviceName(adeviceName);
	SetDeviceVersionNo(adeviceVersionNo);
	SetDeviceAllocators(aAllocator, aDestructor);
}

//----------------------------------------------------------------------------------------
void IDeviceInfo::SetDeviceName(const std::wstring& adeviceName)
{
	SetDeviceNameInternal(adeviceName.c_str());
}
