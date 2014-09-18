//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ISystemInfo::ThisISystemInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
std::wstring ISystemInfo::GetSystemCopyright() const
{
	std::wstring result;
	GetSystemCopyrightInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring ISystemInfo::GetSystemComments() const
{
	std::wstring result;
	GetSystemCommentsInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void ISystemInfo::SetSystemSettings(AllocatorPointer aAllocator, DestructorPointer aDestructor, unsigned int asystemVersionNo, const std::wstring& asystemCopyright, const std::wstring& asystemComments)
{
	SetSystemAllocators(aAllocator, aDestructor);
	SetSystemVersionNo(asystemVersionNo);
	SetSystemCopyright(asystemCopyright);
	SetSystemComments(asystemComments);
}

//----------------------------------------------------------------------------------------
void ISystemInfo::SetSystemCopyright(const std::wstring& asystemCopyright)
{
	SetSystemCopyrightInternal(InteropSupport::STLObjectSource<std::wstring>(asystemCopyright));
}

//----------------------------------------------------------------------------------------
void ISystemInfo::SetSystemComments(const std::wstring& asystemComments)
{
	SetSystemCommentsInternal(InteropSupport::STLObjectSource<std::wstring>(asystemComments));
}
