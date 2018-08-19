#include "GenericAccessGroupSingleSelectionList.h"
#include "IGenericAccessGroup.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupSingleSelectionList::GetIGenericAccessGroupEntryVersion() const
{
	return ThisIGenericAccessGroupEntryVersion();
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupSingleSelectionList::GetIGenericAccessGroupSingleSelectionListVersion() const
{
	return ThisIGenericAccessGroupSingleSelectionListVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessGroupSingleSelectionList::GroupEntryType GenericAccessGroupSingleSelectionList::GetGroupEntryType() const
{
	return GroupEntryType::SingleSelectionList;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupSingleSelectionList::IsGroup() const
{
	return false;
}

//----------------------------------------------------------------------------------------
//Parent functions
//----------------------------------------------------------------------------------------
IGenericAccessGroup* GenericAccessGroupSingleSelectionList::GetParent() const
{
	return _parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupSingleSelectionList::SetParent(IGenericAccessGroup* parent)
{
	_parent = parent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupSingleSelectionList::GetDataContext() const
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
//List info functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupSingleSelectionList::GetDataID() const
{
	return _dataID;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>> GenericAccessGroupSingleSelectionList::GetSelectionList() const
{
	return _selectionList;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupSingleSelectionList::GetAllowNewItemEntry() const
{
	return _allowNewItemEntry;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupSingleSelectionList::GetName() const
{
	return _name;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupSingleSelectionList::GetDescription() const
{
	return _description;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessGroupSingleSelectionList::GetHelpFileLink() const
{
	return _helpFileLink;
}
