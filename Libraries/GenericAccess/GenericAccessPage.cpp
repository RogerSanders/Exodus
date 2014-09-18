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
std::wstring GenericAccessPage::GetName() const
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

//----------------------------------------------------------------------------------------
//Page info methods
//----------------------------------------------------------------------------------------
void GenericAccessPage::GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetName());
}
