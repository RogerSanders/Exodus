#include "GenericAccessGroup.h"

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroup::GetIGenericAccessGroupEntryVersion() const
{
	return ThisIGenericAccessGroupEntryVersion();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroup::GetIGenericAccessGroupVersion() const
{
	return ThisIGenericAccessGroupVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Type functions
//----------------------------------------------------------------------------------------------------------------------
GenericAccessGroup::GroupEntryType GenericAccessGroup::GetGroupEntryType() const
{
	return GroupEntryType::Group;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessGroup::IsGroup() const
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Parent functions
//----------------------------------------------------------------------------------------------------------------------
IGenericAccessGroup* GenericAccessGroup::GetParent() const
{
	return _parent;
}

//----------------------------------------------------------------------------------------------------------------------
void GenericAccessGroup::SetParent(IGenericAccessGroup* parent)
{
	_parent = parent;
}

//----------------------------------------------------------------------------------------------------------------------
// Data context functions
//----------------------------------------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroup::GetDataContext() const
{
	if (_dataContext != 0)
	{
		return _dataContext;
	}
	else if (_parent != 0)
	{
		return _parent->GetDataContext();
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Group info methods
//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroup::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericAccessGroup::GetOpenByDefault() const
{
	return _openByDefault;
}

//----------------------------------------------------------------------------------------------------------------------
// Entry methods
//----------------------------------------------------------------------------------------------------------------------
unsigned int GenericAccessGroup::GetEntryCount() const
{
	return (unsigned int)_childEntries.size();
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::list<IGenericAccessGroupEntry*>> GenericAccessGroup::GetEntries() const
{
	return _childEntries;
}
