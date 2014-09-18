//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessGroupSingleSelectionList::ThisIGenericAccessGroupSingleSelectionListVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//List info functions
//----------------------------------------------------------------------------------------
std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> IGenericAccessGroupSingleSelectionList::GetSelectionList() const
{
	std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> result;
	GetSelectionListInternal(InteropSupport::STLObjectTarget<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupSingleSelectionList::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupSingleSelectionList::GetDescription() const
{
	std::wstring result;
	GetDescriptionInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroupSingleSelectionList::GetHelpFileLink() const
{
	std::wstring result;
	GetHelpFileLinkInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}
