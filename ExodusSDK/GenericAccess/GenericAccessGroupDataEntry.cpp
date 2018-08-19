#include "GenericAccessGroupDataEntry.h"
#include "IGenericAccessGroup.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupDataEntry::GetIGenericAccessGroupEntryVersion() const
{
	return ThisIGenericAccessGroupEntryVersion();
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupDataEntry::GetIGenericAccessGroupDataEntryVersion() const
{
	return ThisIGenericAccessGroupDataEntryVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessGroupDataEntry::GroupEntryType GenericAccessGroupDataEntry::GetGroupEntryType() const
{
	return GroupEntryType::Data;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupDataEntry::IsGroup() const
{
	return false;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IGenericAccessGroup* GenericAccessGroupDataEntry::GetParent() const
{
	return _parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupDataEntry::SetParent(IGenericAccessGroup* parent)
{
	_parent = parent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupDataEntry::GetDataContext() const
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
//Data info functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupDataEntry::GetDataID() const
{
	return _dataID;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupDataEntry::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupDataEntry::GetDescription() const
{
	return _description;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupDataEntry::GetHelpFileLink() const
{
	return _helpFileLink;
}
