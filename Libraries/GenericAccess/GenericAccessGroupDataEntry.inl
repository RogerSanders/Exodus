//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> GenericAccessGroupDataEntry::GenericAccessGroupDataEntry(T adataID, const std::wstring& aname)
:parent(0), dataContext(0), dataID((unsigned int)adataID), name(aname)
{}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry::~GenericAccessGroupDataEntry()
{
	//Delete the data context object we've been assigned
	delete dataContext;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetDataContext(const IGenericAccess::DataContext* adataContext)
{
	dataContext = adataContext;
	return this;
}

//----------------------------------------------------------------------------------------
//Data info functions
//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetName(const std::wstring& aname)
{
	name = aname;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetDescription(const std::wstring& adescription)
{
	description = adescription;
	return this;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetHelpFileLink(const std::wstring& ahelpFileLink)
{
	helpFileLink = ahelpFileLink;
	return this;
}
