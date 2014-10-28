//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> GenericAccessGroupCommandEntry::GenericAccessGroupCommandEntry(T acommandID, const std::wstring& aname)
:parent(0), dataContext(0), commandID((unsigned int)acommandID), name(aname)
{}

//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry::~GenericAccessGroupCommandEntry()
{
	//Delete the data context object we've been assigned
	delete dataContext;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetDataContext(const IGenericAccess::DataContext* adataContext)
{
	dataContext = adataContext;
	return this;
}

//----------------------------------------------------------------------------------------
//Command info methods
//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetName(const std::wstring& aname)
{
	name = aname;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetDescription(const std::wstring& adescription)
{
	description = adescription;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetHelpFileLink(const std::wstring& ahelpFileLink)
{
	helpFileLink = ahelpFileLink;
	return this;
}
