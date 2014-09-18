//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IGenericAccessGroup::ThisIGenericAccessGroupVersion()
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
void IGenericAccessGroup::SetParentForTargetEntry(IGenericAccessGroupEntry* entry, IGenericAccessGroup* aparent) const
{
	entry->SetParent(aparent);
}

//----------------------------------------------------------------------------------------
//Group info functions
//----------------------------------------------------------------------------------------
std::wstring IGenericAccessGroup::GetName() const
{
	std::wstring result;
	GetNameInternal(InteropSupport::STLObjectTarget<std::wstring>(result));
	return result;
}

//----------------------------------------------------------------------------------------
//Entry functions
//----------------------------------------------------------------------------------------
std::list<IGenericAccessGroupEntry*> IGenericAccessGroup::GetEntries() const
{
	std::list<IGenericAccessGroupEntry*> result;
	GetEntriesInternal(InteropSupport::STLObjectTarget<std::list<IGenericAccessGroupEntry*>>(result));
	return result;
}
