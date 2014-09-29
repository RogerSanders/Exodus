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
//Data info functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupSingleSelectionList::GetDataID() const
{
	return dataID;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupSingleSelectionList::GetSelectionListInternal(const InteropSupport::ISTLObjectTarget<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>>& marshaller) const
{
	marshaller.MarshalFrom(GetSelectionList());
}

//----------------------------------------------------------------------------------------
bool GenericAccessGroupSingleSelectionList::GetAllowNewItemEntry() const
{
	return allowNewItemEntry;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupSingleSelectionList::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupSingleSelectionList::GetDescriptionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetDescription());
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupSingleSelectionList::GetHelpFileLinkInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetHelpFileLink());
}
