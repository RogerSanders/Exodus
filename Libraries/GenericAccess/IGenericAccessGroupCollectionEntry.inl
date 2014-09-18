//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct IGenericAccessGroupCollectionEntry::CollectionEntry
{
	CollectionEntry()
	:key(0), value(0)
	{}
	CollectionEntry(IGenericAccessDataValue* akey, IGenericAccessGroupEntry* avalue)
	:key(akey), value(avalue)
	{}

	IGenericAccessDataValue* key;
	IGenericAccessGroupEntry* value;
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessGroupCollectionEntry::ThisIGenericAccessGroupCollectionEntryVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Collection entry functions
//----------------------------------------------------------------------------------------
std::list<IGenericAccessGroupCollectionEntry::CollectionEntry> IGenericAccessGroupCollectionEntry::GetCollectionEntries() const
{
	std::list<CollectionEntry> result;
	GetCollectionEntriesInternal(InteropSupport::STLObjectTarget<std::list<CollectionEntry>>(result));
	return result;
}

//----------------------------------------------------------------------------------------
IGenericAccessGroupEntry* IGenericAccessGroupCollectionEntry::GetCollectionEntry(const std::wstring& key) const
{
	return GetCollectionEntryInternal(InteropSupport::STLObjectSource<std::wstring>(key));
}
