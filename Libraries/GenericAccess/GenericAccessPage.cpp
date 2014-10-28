#include "GenericAccessPage.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessPage::GenericAccessPage(const std::wstring& aname)
:contentRoot(aname)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessPage::GetIGenericAccessPageVersion() const
{
	return ThisIGenericAccessPageVersion();
}

//----------------------------------------------------------------------------------------
//Page info methods
//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessPage::GetName() const
{
	return contentRoot.GetName();
}

//----------------------------------------------------------------------------------------
//Content methods
//----------------------------------------------------------------------------------------
const IGenericAccessGroup* GenericAccessPage::GetContentRoot() const
{
	return &contentRoot;
}

//----------------------------------------------------------------------------------------
GenericAccessPage* GenericAccessPage::AddEntry(IGenericAccessGroupEntry* entry)
{
	contentRoot.AddEntry(entry);
	return this;
}
