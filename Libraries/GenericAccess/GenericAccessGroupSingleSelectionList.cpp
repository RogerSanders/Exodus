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
	return parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupSingleSelectionList::SetParent(IGenericAccessGroup* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupSingleSelectionList::GetDataContext() const
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
//List info functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupSingleSelectionList::GetDataID() const
{
	return dataID;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>> GenericAccessGroupSingleSelectionList::GetSelectionList() const
{
	return selectionList;
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupSingleSelectionList::GetAllowNewItemEntry() const
{
	return allowNewItemEntry;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupSingleSelectionList::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupSingleSelectionList::GetDescription() const
{
	return description;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupSingleSelectionList::GetHelpFileLink() const
{
	return helpFileLink;
}
