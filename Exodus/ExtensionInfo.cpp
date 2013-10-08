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
std::wstring ExtensionInfo::GetExtensionClassName() const
{
	return className;
}

//----------------------------------------------------------------------------------------
std::wstring ExtensionInfo::GetExtensionImplementationName() const
{
	return implementationName;
}

//----------------------------------------------------------------------------------------
unsigned int ExtensionInfo::GetExtensionVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
std::wstring ExtensionInfo::GetExtensionCopyright() const
{
	return copyright;
}

//----------------------------------------------------------------------------------------
std::wstring ExtensionInfo::GetExtensionComments() const
{
	return comments;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor)
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetExtensionClassName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionImplementationNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetExtensionImplementationName(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionVersionNo(unsigned int aextensionVersionNo)
{
	versionNo = aextensionVersionNo;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetExtensionCopyright(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::SetExtensionCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetExtensionComments(marshaller.MarshalTo());
}
