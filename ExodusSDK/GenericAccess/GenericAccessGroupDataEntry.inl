//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> GenericAccessGroupDataEntry::GenericAccessGroupDataEntry(T dataID, const std::wstring& name)
:_parent(0), _dataContext(0), _dataID((unsigned int)dataID), _name(name)
{}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry::~GenericAccessGroupDataEntry()
{
	//Delete the data context object we've been assigned
	delete _dataContext;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetDataContext(const IGenericAccess::DataContext* dataContext)
{
	_dataContext = dataContext;
	return this;
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetName(const std::wstring& name)
{
	_name = name;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetDescription(const std::wstring& description)
{
	_description = description;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetHelpFileLink(const std::wstring& helpFileLink)
{
	_helpFileLink = helpFileLink;
	return this;
}
