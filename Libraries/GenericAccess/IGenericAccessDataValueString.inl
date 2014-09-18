//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessDataValueString::ThisIGenericAccessDataValueStringVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessDataValueString::GetValue() const
{
	std::wstring result;
	GetValueInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
