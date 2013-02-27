#include "ExtensionInfo.h"

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionVersionNo(unsigned int aextensionVersionNo)
{
	extensionVersionNo = aextensionVersionNo;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionAllocators(IExtension* (aAllocator)(const wchar_t*, unsigned int), void (aDestructor)(IExtension*))
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionNameInternal(const wchar_t* aextensionName)
{
	extensionName = aextensionName;
}

//----------------------------------------------------------------------------------------
unsigned int ExtensionInfo::GetExtensionVersionNo() const
{
	return extensionVersionNo;
}

//----------------------------------------------------------------------------------------
std::wstring ExtensionInfo::GetExtensionName() const
{
	return extensionName;
}

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
