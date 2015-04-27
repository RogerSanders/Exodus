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
	return parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupDataEntry::SetParent(IGenericAccessGroup* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupDataEntry::GetDataContext() const
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
unsigned int GenericAccessGroupDataEntry::GetDataID() const
{
	return dataID;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupDataEntry::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupDataEntry::GetDescription() const
{
	return description;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupDataEntry::GetHelpFileLink() const
{
	return helpFileLink;
}
