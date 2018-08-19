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
	return _allocator;
}

//----------------------------------------------------------------------------------------
SystemInfo::DestructorPointer SystemInfo::GetDestructor() const
{
	return _destructor;
}

//----------------------------------------------------------------------------------------
unsigned int SystemInfo::GetSystemVersionNo() const
{
	return _versionNo;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> SystemInfo::GetSystemCopyright() const
{
	return _copyright;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> SystemInfo::GetSystemComments() const
{
	return _comments;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemSettings(AllocatorPointer allocator, DestructorPointer destructor, unsigned int systemVersionNo, const Marshal::In<std::wstring>& systemCopyright, const Marshal::In<std::wstring>& systemComments)
{
	SetSystemAllocators(allocator, destructor);
	SetSystemVersionNo(systemVersionNo);
	SetSystemCopyright(systemCopyright);
	SetSystemComments(systemComments);
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemAllocators(AllocatorPointer allocator, DestructorPointer destructor)
{
	_allocator = allocator;
	_destructor = destructor;
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemVersionNo(unsigned int systemVersionNo)
{
	_versionNo = systemVersionNo;
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemCopyright(const Marshal::In<std::wstring>& systemCopyright)
{
	_copyright = systemCopyright;
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemComments(const Marshal::In<std::wstring>& systemComments)
{
	_comments = systemComments;
}
