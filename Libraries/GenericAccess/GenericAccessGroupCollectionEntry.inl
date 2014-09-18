//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry::GenericAccessGroupCollectionEntry(const std::wstring& aname, IGenericAccessDataValue::DataType akeyDataType)
:parent(0), dataContext(0), name(aname), keyDataType(akeyDataType), openByDefault(false), lastModifiedToken(0)
{}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry::~GenericAccessGroupCollectionEntry()
{
	//Delete the data context object we've been assigned
	delete dataContext;

	//Delete any child content that we've been given ownership of
	for(std::list<CollectionEntry>::const_iterator i = entries.begin(); i != entries.end(); ++i)
	{
		delete i->key;
		delete i->value;
	}
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetDataContext(const IGenericAccess::DataContext* adataContext)
{
	dataContext = adataContext;
	return this;
}

//----------------------------------------------------------------------------------------
//Collection info functions
//----------------------------------------------------------------------------------------
std::wstring GenericAccessGroupCollectionEntry::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetName(const std::wstring& aname)
{
	name = aname;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetOpenByDefault(bool state)
{
	openByDefault = state;
	return this;
}

//----------------------------------------------------------------------------------------
//Collection entry functions
//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetKeyDataType(IGenericAccessDataValue::DataType akeyDataType)
{
	keyDataType = akeyDataType;
	return this;
}

//----------------------------------------------------------------------------------------
std::list<IGenericAccessGroupEntry*> GenericAccessGroupCollectionEntry::GetEntries() const
{
	std::list<IGenericAccessGroupEntry*> entryList;
	for(std::list<GenericAccessGroupCollectionEntry::CollectionEntry>::const_iterator i = entries.begin(); i != entries.end(); ++i)
	{
		entryList.push_back(i->value);
	}
	return entryList;
}

//----------------------------------------------------------------------------------------
std::list<GenericAccessGroupCollectionEntry::CollectionEntry> GenericAccessGroupCollectionEntry::GetCollectionEntries() const
{
	return entries;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::AddCollectionEntry(IGenericAccessDataValue* key, IGenericAccessGroupEntry* value)
{
	return AddCollectionEntry(CollectionEntry(key, value));
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::AddCollectionEntry(const CollectionEntry& entry)
{
	//Set this group as the parent for the target entry
	SetParentForTargetEntry(entry.value, this);

	//Add the new entry to the list of child entries
	entries.push_back(entry);

	//Increment the modification token count so that observers can check if they need to
	//re-iterate the list of entries
	++lastModifiedToken;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::RemoveCollectionEntry(const std::wstring& key)
{
	//Remove the target entry from the list of children if it is currently present
	for(std::list<GenericAccessGroupCollectionEntry::CollectionEntry>::iterator i = entries.begin(); i != entries.end(); ++i)
	{
		const GenericAccessGroupCollectionEntry::CollectionEntry& entry = *i;
		if(entry.key->GetValueString() == key)
		{
			SetParentForTargetEntry(entry.value, 0);
			entries.erase(i);
			break;
		}
	}

	//Increment the modification token count so that observers can check if they need to
	//re-iterate the list of entries
	++lastModifiedToken;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::RemoveCollectionEntry(const IGenericAccessDataValue& key)
{
	return RemoveCollectionEntry(key.GetValueString());
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::RemoveCollectionEntry(IGenericAccessGroupEntry* value)
{
	return this;
}
