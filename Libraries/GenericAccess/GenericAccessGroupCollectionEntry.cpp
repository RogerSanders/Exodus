#include "GenericAccessGroupCollectionEntry.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetIGenericAccessGroupEntryVersion() const
{
	return ThisIGenericAccessGroupEntryVersion();
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetIGenericAccessGroupVersion() const
{
	return ThisIGenericAccessGroupVersion();
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetIGenericAccessGroupCollectionEntryVersion() const
{
	return ThisIGenericAccessGroupCollectionEntryVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessGroupCollectionEntry::GroupEntryType GenericAccessGroupCollectionEntry::GetGroupEntryType() const
{
	return GroupEntryType::Collection;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupCollectionEntry::IsGroup() const
{
	return true;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IGenericAccessGroup* GenericAccessGroupCollectionEntry::GetParent() const
{
	return parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::SetParent(IGenericAccessGroup* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupCollectionEntry::GetDataContext() const
{
	if(dataContext != 0)
	{
		return dataContext;
	}
	else if(parent != 0)
	{
		return parent->GetDataContext();
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Group info functions
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupCollectionEntry::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupCollectionEntry::GetOpenByDefault() const
{
	return openByDefault;
}

//----------------------------------------------------------------------------------------
//Collection entry functions
//----------------------------------------------------------------------------------------
IGenericAccessDataValue::DataType GenericAccessGroupCollectionEntry::GetKeyDataType() const
{
	return keyDataType;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetEntryCount() const
{
	ObtainReadLock();
	unsigned int entryCount = (unsigned int)entries.size();
	ReleaseReadLock();
	return entryCount;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<IGenericAccessGroupEntry*>> GenericAccessGroupCollectionEntry::GetEntries() const
{
	std::list<IGenericAccessGroupEntry*> entryList;
	for(std::list<GenericAccessGroupCollectionEntry::CollectionEntry>::const_iterator i = entries.begin(); i != entries.end(); ++i)
	{
		entryList.push_back(i->value);
	}
	return entryList;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<GenericAccessGroupCollectionEntry::CollectionEntry>> GenericAccessGroupCollectionEntry::GetCollectionEntries() const
{
	return entries;
}

//----------------------------------------------------------------------------------------
IGenericAccessGroupEntry* GenericAccessGroupCollectionEntry::GetCollectionEntry(const MarshalSupport::Marshal::In<std::wstring>& key) const
{
	//##TODO## Consider using a key map to make these lookups more efficient
	std::wstring keyResolved = key.Get();
	for(std::list<CollectionEntry>::const_iterator i = entries.begin(); i != entries.end(); ++i)
	{
		if(i->key->GetValueString() == keyResolved)
		{
			return i->value;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
IGenericAccessGroupEntry* GenericAccessGroupCollectionEntry::GetCollectionEntry(const IGenericAccessDataValue& key) const
{
	return GetCollectionEntry(key.GetValueString());
}

//----------------------------------------------------------------------------------------
//Modification functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCollectionEntry::GetLastModifiedToken() const
{
	return lastModifiedToken;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ObtainReadLock() const
{
	readWriteLock.ObtainReadLock();
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ReleaseReadLock() const
{
	readWriteLock.ReleaseReadLock();
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ObtainWriteLock() const
{
	readWriteLock.ObtainWriteLock();
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::ReleaseWriteLock() const
{
	readWriteLock.ReleaseWriteLock();
}
