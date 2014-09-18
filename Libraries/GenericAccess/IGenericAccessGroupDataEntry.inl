//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessGroupDataEntry::ThisIGenericAccessGroupDataEntryVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupDataEntry::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupDataEntry::GetDescription() const
{
	std::wstring result;
	GetDescriptionInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupDataEntry::GetHelpFileLink() const
{
	std::wstring result;
	GetHelpFileLinkInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
