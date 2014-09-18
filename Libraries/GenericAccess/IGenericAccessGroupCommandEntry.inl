//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessGroupCommandEntry::ThisIGenericAccessGroupCommandEntryVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupCommandEntry::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupCommandEntry::GetDescription() const
{
	std::wstring result;
	GetDescriptionInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupCommandEntry::GetHelpFileLink() const
{
	std::wstring result;
	GetHelpFileLinkInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
