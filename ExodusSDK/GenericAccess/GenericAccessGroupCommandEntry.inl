//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
template<class T> GenericAccessGroupCommandEntry::GenericAccessGroupCommandEntry(T commandID, const std::wstring& name)
:_parent(0), _dataContext(0), _commandID((unsigned int)commandID), _name(name)
{}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry::~GenericAccessGroupCommandEntry()
{
	// Delete the data context object we've been assigned
	delete _dataContext;
}

//----------------------------------------------------------------------------------------------------------------------
// Data context functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetDataContext(const IGenericAccess::DataContext* dataContext)
{
	_dataContext = dataContext;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// Command info methods
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetName(const std::wstring& name)
{
	_name = name;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetDescription(const std::wstring& description)
{
	_description = description;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetHelpFileLink(const std::wstring& helpFileLink)
{
	_helpFileLink = helpFileLink;
	return this;
}
