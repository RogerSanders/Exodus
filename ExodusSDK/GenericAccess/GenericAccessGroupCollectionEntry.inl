//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry::GenericAccessGroupCollectionEntry(const std::wstring& name, IGenericAccessDataValue::DataType keyDataType)
:_parent(0), _dataContext(0), _name(name), _keyDataType(keyDataType), _openByDefault(false), _lastModifiedToken(0)
{}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry::~GenericAccessGroupCollectionEntry()
{
	// Delete the data context object we've been assigned
	delete _dataContext;

	// Delete any child content that we've been given ownership of
	for (std::list<CollectionEntry>::const_iterator i = _entries.begin(); i != _entries.end(); ++i)
	{
		delete i->key;
		delete i->value;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Data context functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetDataContext(const IGenericAccess::DataContext* dataContext)
{
	_dataContext = dataContext;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// Collection info functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetName(const std::wstring& name)
{
	_name = name;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetOpenByDefault(bool state)
{
	_openByDefault = state;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
// Collection entry functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::SetKeyDataType(IGenericAccessDataValue::DataType keyDataType)
{
	_keyDataType = keyDataType;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::AddCollectionEntry(IGenericAccessDataValue* key, IGenericAccessGroupEntry* value)
{
	return AddCollectionEntry(CollectionEntry(key, value));
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::AddCollectionEntry(const CollectionEntry& entry)
{
	// Set this group as the parent for the target entry
	SetParentForTargetEntry(entry.value, this);

	// Add the new entry to the list of child entries
	_entries.push_back(entry);

	// Increment the modification token count so that observers can check if they need to
	// re-iterate the list of entries
	++_lastModifiedToken;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::RemoveCollectionEntry(const std::wstring& key)
{
	// Remove the target entry from the list of children if it is currently present
	for (std::list<GenericAccessGroupCollectionEntry::CollectionEntry>::iterator i = _entries.begin(); i != _entries.end(); ++i)
	{
		const GenericAccessGroupCollectionEntry::CollectionEntry& entry = *i;
		if (entry.key->GetValueString() == key)
		{
			SetParentForTargetEntry(entry.value, 0);
			_entries.erase(i);
			break;
		}
	}

	// Increment the modification token count so that observers can check if they need to
	// re-iterate the list of entries
	++_lastModifiedToken;
	return this;
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::RemoveCollectionEntry(const IGenericAccessDataValue& key)
{
	return RemoveCollectionEntry(key.GetValueString());
}

//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry* GenericAccessGroupCollectionEntry::RemoveCollectionEntry(IGenericAccessGroupEntry* value)
{
	return this;
}
