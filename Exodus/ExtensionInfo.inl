//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& aextensionClassName, const std::wstring& aextensionImplementationName, unsigned int aextensionVersionNo, const std::wstring& aextensionCopyright, const std::wstring& aextensionComments)
{
	SetExtensionAllocators(aAllocator, aDestructor);
	SetExtensionClassName(aextensionClassName);
	SetExtensionImplementationName(aextensionImplementationName);
	SetExtensionVersionNo(aextensionVersionNo);
	SetExtensionCopyright(aextensionCopyright);
	SetExtensionComments(aextensionComments);
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionClassName(const std::wstring& aextensionClassName)
{
	className = aextensionClassName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionImplementationName(const std::wstring& aextensionImplementationName)
{
	implementationName = aextensionImplementationName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionCopyright(const std::wstring& aextensionCopyright)
{
	copyright = aextensionCopyright;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionComments(const std::wstring& aextensionComments)
{
	comments = aextensionComments;
}
