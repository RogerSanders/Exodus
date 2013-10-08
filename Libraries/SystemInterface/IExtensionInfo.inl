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
void IExtensionInfo::SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const std::wstring& aextensionClassName, const std::wstring& aextensionImplementationName, unsigned int aextensionVersionNo, const std::wstring& aextensionCopyright, const std::wstring& aextensionComments)
{
	SetExtensionAllocators(aAllocator, aDestructor);
	SetExtensionClassName(aextensionClassName);
	SetExtensionImplementationName(aextensionImplementationName);
	SetExtensionVersionNo(aextensionVersionNo);
	SetExtensionCopyright(aextensionCopyright);
	SetExtensionComments(aextensionComments);
}

//----------------------------------------------------------------------------------------
void IExtensionInfo::SetExtensionClassName(const std::wstring& aextensionClassName)
{
	SetExtensionClassNameInternal(InteropSupport::STLObjectSource<std::wstring>(aextensionClassName));
}

//----------------------------------------------------------------------------------------
void IExtensionInfo::SetExtensionImplementationName(const std::wstring& aextensionImplementationName)
{
	SetExtensionImplementationNameInternal(InteropSupport::STLObjectSource<std::wstring>(aextensionImplementationName));
}

//----------------------------------------------------------------------------------------
void IExtensionInfo::SetExtensionCopyright(const std::wstring& aextensionCopyright)
{
	SetExtensionCopyrightInternal(InteropSupport::STLObjectSource<std::wstring>(aextensionCopyright));
}

//----------------------------------------------------------------------------------------
void IExtensionInfo::SetExtensionComments(const std::wstring& aextensionComments)
{
	SetExtensionCommentsInternal(InteropSupport::STLObjectSource<std::wstring>(aextensionComments));
}
