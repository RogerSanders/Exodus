//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IExtensionInfo::ThisIExtensionInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void IExtensionInfo::SetExtensionSettings(const std::wstring& aextensionName, AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int aextensionVersionNo)
{
	SetExtensionName(aextensionName);
	SetExtensionVersionNo(aextensionVersionNo);
	SetExtensionAllocators(aAllocator, aDestructor);
}

//----------------------------------------------------------------------------------------
void IExtensionInfo::SetExtensionName(const std::wstring& aextensionName)
{
	SetExtensionNameInternal(aextensionName.c_str());
}
