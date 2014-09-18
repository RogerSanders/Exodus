//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessDataInfo::ThisIGenericAccessDataInfoVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//File path settings
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessDataInfo::GetFilePathExtensionFilter() const
{
	std::wstring result;
	GetFilePathExtensionFilterInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessDataInfo::GetFilePathDefaultExtension() const
{
	std::wstring result;
	GetFilePathDefaultExtensionInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
