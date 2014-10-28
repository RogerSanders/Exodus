#include "ExtensionInfo.h"

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
MarshalSupport::Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionImplementationName() const
{
	return implementationName;
}

//----------------------------------------------------------------------------------------
unsigned int ExtensionInfo::GetExtensionVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionCopyright() const
{
	return copyright;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExtensionInfo::GetExtensionComments() const
{
	return comments;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, const MarshalSupport::Marshal::In<std::wstring>& aextensionClassName, const MarshalSupport::Marshal::In<std::wstring>& aextensionImplementationName, unsigned int aextensionVersionNo, const MarshalSupport::Marshal::In<std::wstring>& aextensionCopyright, const MarshalSupport::Marshal::In<std::wstring>& aextensionComments)
{
	SetExtensionAllocators(aAllocator, aDestructor);
	SetExtensionClassName(aextensionClassName);
	SetExtensionImplementationName(aextensionImplementationName);
	SetExtensionVersionNo(aextensionVersionNo);
	SetExtensionCopyright(aextensionCopyright);
	SetExtensionComments(aextensionComments);
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor)
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionClassName(const MarshalSupport::Marshal::In<std::wstring>& aextensionClassName)
{
	className = aextensionClassName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionImplementationName(const MarshalSupport::Marshal::In<std::wstring>& aextensionImplementationName)
{
	implementationName = aextensionImplementationName;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionVersionNo(unsigned int aextensionVersionNo)
{
	versionNo = aextensionVersionNo;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionCopyright(const MarshalSupport::Marshal::In<std::wstring>& aextensionCopyright)
{
	copyright = aextensionCopyright;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionComments(const MarshalSupport::Marshal::In<std::wstring>& aextensionComments)
{
	comments = aextensionComments;
}
