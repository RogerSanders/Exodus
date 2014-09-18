//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessDataValueFilePath::ThisIGenericAccessDataValueFilePathVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessDataValueFilePath::GetValue() const
{
	std::wstring result;
	GetValueInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
