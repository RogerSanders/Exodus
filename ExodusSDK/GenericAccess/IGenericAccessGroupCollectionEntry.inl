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
