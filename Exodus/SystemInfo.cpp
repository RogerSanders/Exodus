#include "SystemInfo.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int SystemInfo::GetISystemInfoVersion() const
{
	return ThisISystemInfoVersion();
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
SystemInfo::AllocatorPointer SystemInfo::GetAllocator() const
{
	return Allocator;
}

//----------------------------------------------------------------------------------------
SystemInfo::DestructorPointer SystemInfo::GetDestructor() const
{
	return Destructor;
}

//----------------------------------------------------------------------------------------
unsigned int SystemInfo::GetSystemVersionNo() const
{
	return versionNo;
}

//----------------------------------------------------------------------------------------
void SystemInfo::GetSystemCopyrightInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetSystemCopyright());
}

//----------------------------------------------------------------------------------------
void SystemInfo::GetSystemCommentsInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetSystemComments());
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemAllocators(AllocatorPointer aAllocator, DestructorPointer aDestructor)
{
	Allocator = aAllocator;
	Destructor = aDestructor;
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemVersionNo(unsigned int asystemVersionNo)
{
	versionNo = asystemVersionNo;
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemCopyrightInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetSystemCopyright(marshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemCommentsInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller)
{
	SetSystemComments(marshaller.MarshalTo());
}
