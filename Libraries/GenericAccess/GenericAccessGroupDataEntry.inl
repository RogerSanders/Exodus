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
std::wstring GenericAccessGroupDataEntry::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetName(const std::wstring& aname)
{
	name = aname;
	return this;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessGroupDataEntry::GetDescription() const
{
	return description;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetDescription(const std::wstring& adescription)
{
	description = adescription;
	return this;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessGroupDataEntry::GetHelpFileLink() const
{
	return helpFileLink;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry* GenericAccessGroupDataEntry::SetHelpFileLink(const std::wstring& ahelpFileLink)
{
	helpFileLink = ahelpFileLink;
	return this;
}
