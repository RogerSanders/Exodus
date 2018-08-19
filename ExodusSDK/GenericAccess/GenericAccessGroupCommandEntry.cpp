#include "GenericAccessGroupCommandEntry.h"
#include "IGenericAccessGroup.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCommandEntry::GetIGenericAccessGroupEntryVersion() const
{
	return ThisIGenericAccessGroupEntryVersion();
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCommandEntry::GetIGenericAccessGroupCommandEntryVersion() const
{
	return ThisIGenericAccessGroupCommandEntryVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessGroupCommandEntry::GroupEntryType GenericAccessGroupCommandEntry::GetGroupEntryType() const
{
	return GroupEntryType::Command;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupCommandEntry::IsGroup() const
{
	return false;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IGenericAccessGroup* GenericAccessGroupCommandEntry::GetParent() const
{
	return _parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCommandEntry::SetParent(IGenericAccessGroup* parent)
{
	_parent = parent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupCommandEntry::GetDataContext() const
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

//----------------------------------------------------------------------------------------
//Command info methods
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCommandEntry::GetCommandID() const
{
	return _commandID;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupCommandEntry::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupCommandEntry::GetDescription() const
{
	return _description;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupCommandEntry::GetHelpFileLink() const
{
	return _helpFileLink;
}
