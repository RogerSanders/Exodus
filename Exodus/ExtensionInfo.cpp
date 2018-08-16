#include "ExtensionInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExtensionInfo::ExtensionInfo()
:persistentGlobalExtension(false)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ExtensionInfo::GetIExtensionInfoVersion() const
{
	return ThisIExtensionInfoVersion();
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
ExtensionInfo::AllocatorPointer ExtensionInfo::GetAllocator() const
{
	return Allocator;
}

//----------------------------------------------------------------------------------------
ExtensionInfo::DestructorPointer ExtensionInfo::GetDestructor() const
{
	return Destructor;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionImplementationName() const
{
	return implementationName;
}

//----------------------------------------------------------------------------------------
unsigned int ExtensionInfo::GetExtensionVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionCopyright() const
{
	return copyright;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionComments() const
{
	return comments;
}

//----------------------------------------------------------------------------------------
bool ExtensionInfo::GetIsPersistentGlobalExtension() const
{
	return persistentGlobalExtension;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const Marshal::In<std::wstring>& aextensionClassName, const Marshal::In<std::wstring>& aextensionImplementationName, unsigned int aextensionVersionNo, const Marshal::In<std::wstring>& aextensionCopyright, const Marshal::In<std::wstring>& aextensionComments, bool apersistentGlobalExtension)
{
	SetExtensionAllocators(aAllocator, aDestructor);
	SetExtensionClassName(aextensionClassName);
	SetExtensionImplementationName(aextensionImplementationName);
	SetExtensionVersionNo(aextensionVersionNo);
	SetExtensionCopyright(aextensionCopyright);
	SetExtensionComments(aextensionComments);
	SetIsPersistentGlobalExtension(apersistentGlobalExtension);
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor)
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionClassName(const Marshal::In<std::wstring>& aextensionClassName)
{
	className = aextensionClassName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionImplementationName(const Marshal::In<std::wstring>& aextensionImplementationName)
{
	implementationName = aextensionImplementationName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionVersionNo(unsigned int aextensionVersionNo)
{
	versionNo = aextensionVersionNo;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionCopyright(const Marshal::In<std::wstring>& aextensionCopyright)
{
	copyright = aextensionCopyright;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionComments(const Marshal::In<std::wstring>& aextensionComments)
{
	comments = aextensionComments;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetIsPersistentGlobalExtension(bool state)
{
	persistentGlobalExtension = state;
}
