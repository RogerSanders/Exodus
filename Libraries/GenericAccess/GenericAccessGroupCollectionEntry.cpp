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
	return GROUPENTRYTYPE_COLLECTION;
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
void GenericAccessGroupCollectionEntry::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
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
void GenericAccessGroupCollectionEntry::GetEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<IGenericAccessGroupEntry*>>& marshaller) const
{
	marshaller.MarshalFrom(GetEntries());
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCollectionEntry::GetCollectionEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<CollectionEntry>>& marshaller) const
{
	marshaller.MarshalFrom(GetCollectionEntries());
}

//----------------------------------------------------------------------------------------
IGenericAccessGroupEntry* GenericAccessGroupCollectionEntry::GetCollectionEntryInternal(const InteropSupport::ISTLObjectSource<std::wstring>& keyMarshaller) const
{
	return GetCollectionEntry(keyMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IGenericAccessGroupEntry* GenericAccessGroupCollectionEntry::GetCollectionEntry(const std::wstring& key) const
{
	//##TODO## Consider using a key map to make these lookups more efficient
	for(std::list<CollectionEntry>::const_iterator i = entries.begin(); i != entries.end(); ++i)
	{
		if(i->key->GetValueString() == key)
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
