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
void ExtensionInfo::GetExtensionClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetExtensionClassName());
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::GetExtensionImplementationNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetExtensionImplementationName());
}

//----------------------------------------------------------------------------------------
unsigned int ExtensionInfo::GetExtensionVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::GetExtensionCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetExtensionCopyright());
}

//----------------------------------------------------------------------------------------
void ExtensionInfo::GetExtensionCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetExtensionComments());
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
