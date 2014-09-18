//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
std::wstring SystemInfo::GetSystemCopyright() const
{
	return copyright;
}

//----------------------------------------------------------------------------------------
std::wstring SystemInfo::GetSystemComments() const
{
	return comments;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int asystemVersionNo, const std::wstring& asystemCopyright, const std::wstring& asystemComments)
{
	SetSystemAllocators(aAllocator, aDestructor);
	SetSystemVersionNo(asystemVersionNo);
	SetSystemCopyright(asystemCopyright);
	SetSystemComments(asystemComments);
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemCopyright(const std::wstring& asystemCopyright)
{
	copyright = asystemCopyright;
}

//----------------------------------------------------------------------------------------
void SystemInfo::SetSystemComments(const std::wstring& asystemComments)
{
	comments = asystemComments;
}
