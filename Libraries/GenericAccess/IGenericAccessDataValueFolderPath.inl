//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessDataValueFolderPath::ThisIGenericAccessDataValueFolderPathVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessDataValueFolderPath::GetValue() const
{
	std::wstring result;
	GetValueInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
