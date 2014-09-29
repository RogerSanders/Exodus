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
std::wstring GenericAccessGroupCommandEntry::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetName(const std::wstring& aname)
{
	name = aname;
	return this;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessGroupCommandEntry::GetDescription() const
{
	return description;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetDescription(const std::wstring& adescription)
{
	description = adescription;
	return this;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessGroupCommandEntry::GetHelpFileLink() const
{
	return helpFileLink;
}

//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry* GenericAccessGroupCommandEntry::SetHelpFileLink(const std::wstring& ahelpFileLink)
{
	helpFileLink = ahelpFileLink;
	return this;
}
