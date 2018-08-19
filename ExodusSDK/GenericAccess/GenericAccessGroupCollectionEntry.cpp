#include "GenericAccessGroupCollectionEntry.h"

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetIGenericAccessGroupEntryVersion() const
{
	return ThisIGenericAccessGroupEntryVersion();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetIGenericAccessGroupVersion() const
{
	return ThisIGenericAccessGroupVersion();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetIGenericAccessGroupCollectionEntryVersion() const
{
	return ThisIGenericAccessGroupCollectionEntryVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Type functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry::GroupEntryType GenericAccessGroupCollectionEntry::GetGroupEntryType() const
{
	return GroupEntryType::Collection;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessGroupCollectionEntry::IsGroup() const
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Parent functions
//----------------------------------------------------------------------------------------------------------------------
IGenericAccessGroup* GenericAccessGroupCollectionEntry::GetParent() const
{
	return _parent;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::SetParent(IGenericAccessGroup* parent)
{
	_parent = parent;
}

//----------------------------------------------------------------------------------------------------------------------
// Data context functions
//----------------------------------------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupCollectionEntry::GetDataContext() const
{
	if (_dataContext != 0)
	{
		return _dataContext;
	}
	else if (_parent != 0)
	{
		return _parent->GetDataContext();
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Group info functions
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupCollectionEntry::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessGroupCollectionEntry::GetOpenByDefault() const
{
	return _openByDefault;
}

//----------------------------------------------------------------------------------------------------------------------
// Collection entry functions
//----------------------------------------------------------------------------------------------------------------------
IGenericAccessDataValue::DataType GenericAccessGroupCollectionEntry::GetKeyDataType() const
{
	return _keyDataType;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetEntryCount() const
{
	ObtainReadLock();
	unsigned int entryCount = (unsigned int)_entries.size();
	ReleaseReadLock();
	return entryCount;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::list<IGenericAccessGroupEntry*>> GenericAccessGroupCollectionEntry::GetEntries() const
{
	std::list<IGenericAccessGroupEntry*> entryList;
	for (std::list<GenericAccessGroupCollectionEntry::CollectionEntry>::const_iterator i = _entries.begin(); i != _entries.end(); ++i)
	{
		entryList.push_back(i->value);
	}
	return entryList;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::list<GenericAccessGroupCollectionEntry::CollectionEntry>> GenericAccessGroupCollectionEntry::GetCollectionEntries() const
{
	return _entries;
}

//----------------------------------------------------------------------------------------------------------------------
IGenericAccessGroupEntry* GenericAccessGroupCollectionEntry::GetCollectionEntry(const Marshal::In<std::wstring>& key) const
{
	//##TODO## Consider using a key map to make these lookups more efficient
	std::wstring keyResolved = key.Get();
	for (std::list<CollectionEntry>::const_iterator i = _entries.begin(); i != _entries.end(); ++i)
	{
		if (i->key->GetValueString() == keyResolved)
		{
			return i->value;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
IGenericAccessGroupEntry* GenericAccessGroupCollectionEntry::GetCollectionEntry(const IGenericAccessDataValue& key) const
{
	return GetCollectionEntry(key.GetValueString());
}

//----------------------------------------------------------------------------------------------------------------------
// Modification functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetLastModifiedToken() const
{
	return _lastModifiedToken;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ObtainReadLock() const
{
	_readWriteLock.ObtainReadLock();
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ReleaseReadLock() const
{
	_readWriteLock.ReleaseReadLock();
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ObtainWriteLock() const
{
	_readWriteLock.ObtainWriteLock();
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ReleaseWriteLock() const
{
	_readWriteLock.ReleaseWriteLock();
}
