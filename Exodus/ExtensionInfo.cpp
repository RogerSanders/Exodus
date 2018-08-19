#include "ExtensionInfo.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExtensionInfo::ExtensionInfo()
:_persistentGlobalExtension(false)
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
	return _allocator;
}

//----------------------------------------------------------------------------------------
ExtensionInfo::DestructorPointer ExtensionInfo::GetDestructor() const
{
	return _destructor;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionClassName() const
{
	return _className;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionImplementationName() const
{
	return _implementationName;
}

//----------------------------------------------------------------------------------------
unsigned int ExtensionInfo::GetExtensionVersionNo() const
{
	return _versionNo;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionCopyright() const
{
	return _copyright;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionComments() const
{
	return _comments;
}

//----------------------------------------------------------------------------------------
bool ExtensionInfo::GetIsPersistentGlobalExtension() const
{
	return _persistentGlobalExtension;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionSettings(AllocatorPointer allocator, DestructorPointer destructor, const Marshal::In<std::wstring>& extensionClassName, const Marshal::In<std::wstring>& extensionImplementationName, unsigned int extensionVersionNo, const Marshal::In<std::wstring>& extensionCopyright, const Marshal::In<std::wstring>& extensionComments, bool persistentGlobalExtension)
{
	SetExtensionAllocators(allocator, destructor);
	SetExtensionClassName(extensionClassName);
	SetExtensionImplementationName(extensionImplementationName);
	SetExtensionVersionNo(extensionVersionNo);
	SetExtensionCopyright(extensionCopyright);
	SetExtensionComments(extensionComments);
	SetIsPersistentGlobalExtension(persistentGlobalExtension);
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionAllocators(AllocatorPointer allocator, DestructorPointer destructor)
{
	_allocator = allocator;
	_destructor = destructor;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionClassName(const Marshal::In<std::wstring>& extensionClassName)
{
	_className = extensionClassName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionImplementationName(const Marshal::In<std::wstring>& extensionImplementationName)
{
	_implementationName = extensionImplementationName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionVersionNo(unsigned int extensionVersionNo)
{
	_versionNo = extensionVersionNo;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionCopyright(const Marshal::In<std::wstring>& extensionCopyright)
{
	_copyright = extensionCopyright;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionComments(const Marshal::In<std::wstring>& extensionComments)
{
	_comments = extensionComments;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetIsPersistentGlobalExtension(bool state)
{
	_persistentGlobalExtension = state;
}
