//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
template<class T>
GenericAccessGroupSingleSelectionList::GenericAccessGroupSingleSelectionList(T dataID, const std::wstring& name)
:_parent(0), _dataContext(0), _dataID((unsigned int)dataID), _allowNewItemEntry(false), _name(name)
{ }

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList::~GenericAccessGroupSingleSelectionList()
{
	// Delete the data context object we've been assigned
	delete _dataContext;

	// Delete any key/value pairs we've been given ownership of
	for (std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>::const_iterator i = _selectionList.begin(); i != _selectionList.end(); ++i)
	{
		delete i->first;
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Data context functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetDataContext(const IGenericAccess::DataContext* dataContext)
{
	_dataContext = dataContext;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// List info functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetSelectionList(const std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>& selectionList)
{
	_selectionList = selectionList;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::AddSelectionListEntry(const IGenericAccessDataValue* key, const IGenericAccessDataValue* value)
{
	// In the case that value has not been defined, use the key as the value.
	value = (value == 0)? key: value;

	// Add the specified key/value pair to the selection list
	_selectionList.push_back(std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>(key, value));
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetAllowNewItemEntry(bool state)
{
	_allowNewItemEntry = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetName(const std::wstring& name)
{
	_name = name;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetDescription(const std::wstring& description)
{
	_description = description;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetHelpFileLink(const std::wstring& helpFileLink)
{
	_helpFileLink = helpFileLink;
	return this;
}
