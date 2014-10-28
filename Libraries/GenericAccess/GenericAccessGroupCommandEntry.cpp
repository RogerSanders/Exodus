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
	return parent;
}

//----------------------------------------------------------------------------------------
void GenericAccessGroupCommandEntry::SetParent(IGenericAccessGroup* aparent)
{
	parent = aparent;
}

//----------------------------------------------------------------------------------------
//Data context functions
//----------------------------------------------------------------------------------------
const IGenericAccess::DataContext* GenericAccessGroupCommandEntry::GetDataContext() const
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
//Command info methods
//----------------------------------------------------------------------------------------
unsigned int GenericAccessGroupCommandEntry::GetCommandID() const
{
	return commandID;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupCommandEntry::GetName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupCommandEntry::GetDescription() const
{
	return description;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessGroupCommandEntry::GetHelpFileLink() const
{
	return helpFileLink;
}
