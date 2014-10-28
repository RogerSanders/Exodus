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
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroup::GetName() const
{
	return name;
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
MarshalSupport::Marshal::Ret<std::list<IGenericAccessGroupEntry*>> GenericAccessGroup::GetEntries() const
{
	return childEntries;
}
