//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessPage::ThisIGenericAccessPageVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Page info functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessPage::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
