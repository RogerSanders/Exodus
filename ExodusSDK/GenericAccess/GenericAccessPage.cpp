#include "GenericAccessPage.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessPage::GenericAccessPage(const std::wstring& aname, const std::wstring& atitle)
:contentRoot(aname)
{
	title = (atitle.empty())? aname: atitle;
}

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
MarshalSupport::Marshal::Ret<std::wstring> GenericAccessPage::GetTitle() const
{
	return title;
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
