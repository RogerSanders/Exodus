//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void IExtensionInfo::SetExtensionSettings(const std::wstring& aextensionName, IExtension* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IExtension*), unsigned int aextensionVersionNo)
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
