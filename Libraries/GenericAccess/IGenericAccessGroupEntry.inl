//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum IGenericAccessGroupEntry::GroupEntryType
{
	GROUPENTRYTYPE_GROUP,
	GROUPENTRYTYPE_DATA,
	GROUPENTRYTYPE_COMMAND,
	GROUPENTRYTYPE_SINGLESELECTIONLIST,
	GROUPENTRYTYPE_COLLECTION
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessGroupEntry::ThisIGenericAccessGroupEntryVersion()
{
	return 1;
}
