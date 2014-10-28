//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> GenericAccessGroupSingleSelectionList::GenericAccessGroupSingleSelectionList(T adataID, const std::wstring& aname)
:parent(0), dataContext(0), dataID((unsigned int)adataID), allowNewItemEntry(false), name(aname)
{}

//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList::~GenericAccessGroupSingleSelectionList()
{
	//Delete the data context object we've been assigned
	delete dataContext;

	//Delete any key/value pairs we've been given ownership of
	for(std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>::const_iterator i = selectionList.begin(); i != selectionList.end(); ++i)
	{
		delete i->first;
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetDataContext(const IGenericAccess::DataContext* adataContext)
{
	dataContext = adataContext;
	return this;
}

//----------------------------------------------------------------------------------------
//List info functions
//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetSelectionList(const std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>& aselectionList)
{
	selectionList = aselectionList;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::AddSelectionListEntry(const IGenericAccessDataValue* key, const IGenericAccessDataValue* value)
{
	//In the case that value has not been defined, use the key as the value.
	value = (value == 0)? key: value;

	//Add the specified key/value pair to the selection list
	selectionList.push_back(std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>(key, value));
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetAllowNewItemEntry(bool state)
{
	allowNewItemEntry = state;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetName(const std::wstring& aname)
{
	name = aname;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetDescription(const std::wstring& adescription)
{
	description = adescription;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList* GenericAccessGroupSingleSelectionList::SetHelpFileLink(const std::wstring& ahelpFileLink)
{
	helpFileLink = ahelpFileLink;
	return this;
}
