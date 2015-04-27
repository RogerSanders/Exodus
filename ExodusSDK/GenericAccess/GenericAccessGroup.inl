//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessGroup::GenericAccessGroup(const std::wstring& aname)
:parent(0), dataContext(0), name(aname), openByDefault(true)
{}

//----------------------------------------------------------------------------------------
GenericAccessGroup::~GenericAccessGroup()
{
	//Delete the data context object we've been assigned
	delete dataContext;

	//Delete any child content that we've been given ownership of
	for(std::list<IGenericAccessGroupEntry*>::const_iterator i = childEntries.begin(); i != childEntries.end(); ++i)
	{
		delete *i;
	}
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::SetDataContext(const IGenericAccess::DataContext* adataContext)
{
	dataContext = adataContext;
	return this;
}

//----------------------------------------------------------------------------------------
//Group info methods
//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::SetName(const std::wstring& aname)
{
	name = aname;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::SetOpenByDefault(bool state)
{
	openByDefault = state;
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
	childEntries.push_back(entry);
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroup* GenericAccessGroup::RemoveEntry(IGenericAccessGroupEntry* entry)
{
	//Remove the target entry from the list of children if it is currently present
	for(std::list<IGenericAccessGroupEntry*>::iterator i = childEntries.begin(); i != childEntries.end(); ++i)
	{
		if(*i == entry)
		{
			SetParentForTargetEntry(entry, 0);
			childEntries.erase(i);
			break;
		}
	}
	return this;
}
