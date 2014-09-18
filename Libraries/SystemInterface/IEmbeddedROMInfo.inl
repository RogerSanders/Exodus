//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IEmbeddedROMInfo::ThisIEmbeddedROMInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
std::wstring IEmbeddedROMInfo::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IEmbeddedROMInfo::GetDisplayName() const
{
	std::wstring result;
	GetDisplayNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IEmbeddedROMInfo::GetFilePath() const
{
	std::wstring result;
	GetFilePathInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void IEmbeddedROMInfo::SetName(const std::wstring& name)
{
	SetNameInternal(InteropSupport::STLObjectSource<std::wstring>(name));
}

//----------------------------------------------------------------------------------------
void IEmbeddedROMInfo::SetDisplayName(const std::wstring& displayName)
{
	SetDisplayNameInternal(InteropSupport::STLObjectSource<std::wstring>(displayName));
}

//----------------------------------------------------------------------------------------
void IEmbeddedROMInfo::SetFilePath(const std::wstring& filePath)
{
	SetFilePathInternal(InteropSupport::STLObjectSource<std::wstring>(filePath));
}
