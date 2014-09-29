#include "GenericAccessGroup.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroup::GetIGenericAccessGroupEntryVersion() const
{
	return ThisIGenericAccessGroupEntryVersion();
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroup::GetIGenericAccessGroupVersion() const
{
	return ThisIGenericAccessGroupVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessGroup::GroupEntryType GenericAccessGroup::GetGroupEntryType() const
{
	return GroupEntryType::Group;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroup::IsGroup() const
{
	return true;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IGenericAccessGroup* GenericAccessGroup::GetParent() const
{
	return parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroup::SetParent(IGenericAccessGroup* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroup::GetDataContext() const
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
//Group info methods
//----------------------------------------------------------------------------------------
void GenericAccessGroup::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroup::GetOpenByDefault() const
{
	return openByDefault;
}

//----------------------------------------------------------------------------------------
//Entry methods
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroup::GetEntryCount() const
{
	return (unsigned int)childEntries.size();
}

//----------------------------------------------------------------------------------------
void GenericAccessGroup::GetEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<IGenericAccessGroupEntry*>>& marshaller) const
{
	marshaller.MarshalFrom(GetEntries());
}
