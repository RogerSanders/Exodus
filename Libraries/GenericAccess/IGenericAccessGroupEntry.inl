//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IGenericAccessGroupEntry::GroupEntryType
{
	Group,
	Data,
	Command,
	SingleSelectionList,
	Collection
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessGroupEntry::ThisIGenericAccessGroupEntryVersion()
{
	return 1;
}
