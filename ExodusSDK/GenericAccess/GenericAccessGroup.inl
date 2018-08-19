//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessGroup::GenericAccessGroup(const std::wstring& name)
:_parent(0), _dataContext(0), _name(name), _openByDefault(true)
{}

//----------------------------------------------------------------------------------------
GenericAccessGroup::~GenericAccessGroup()
{
	//Delete the data context object we've been assigned
	delete _dataContext;

	//Delete any child content that we've been given ownership of
	for (std::list<IGenericAccessGroupEntry*>::const_iterator i = _childEntries.begin(); i != _childEntries.end(); ++i)
	{
		delete *i;
	}
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::SetDataContext(const IGenericAccess::DataContext* dataContext)
{
	_dataContext = dataContext;
	return this;
}

//----------------------------------------------------------------------------------------
//Group info methods
//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::SetName(const std::wstring& name)
{
	_name = name;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::SetOpenByDefault(bool state)
{
	_openByDefault = state;
	return this;
}

//----------------------------------------------------------------------------------------
//Entry methods
//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::AddEntry(IGenericAccessGroupEntry* entry)
{
	//Set this group as the parent for the target entry
	SetParentForTargetEntry(entry, this);

	//Add the new entry to the list of child entries
	_childEntries.push_back(entry);
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::RemoveEntry(IGenericAccessGroupEntry* entry)
{
	//Remove the target entry from the list of children if it is currently present
	for (std::list<IGenericAccessGroupEntry*>::iterator i = _childEntries.begin(); i != _childEntries.end(); ++i)
	{
		if (*i == entry)
		{
			SetParentForTargetEntry(entry, 0);
			_childEntries.erase(i);
			break;
		}
	}
	return this;
}
