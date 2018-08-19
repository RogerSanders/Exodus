#include "GenericAccessPage.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessPage::GenericAccessPage(const std::wstring& name, const std::wstring& title, Type type)
:_contentRoot(name), _type(type)
{
	_title = (title.empty())? name: title;
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
GenericAccessPage::Type GenericAccessPage::GetPageType() const
{
	return _type;
}

//----------------------------------------------------------------------------------------
void GenericAccessPage::SetPageType(Type type)
{
	_type = type;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessPage::GetName() const
{
	return _contentRoot.GetName();
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> GenericAccessPage::GetTitle() const
{
	return _title;
}

//----------------------------------------------------------------------------------------
//Content methods
//----------------------------------------------------------------------------------------
const IGenericAccessGroup* GenericAccessPage::GetContentRoot() const
{
	return &_contentRoot;
}

//----------------------------------------------------------------------------------------
GenericAccessPage* GenericAccessPage::AddEntry(IGenericAccessGroupEntry* entry)
{
	_contentRoot.AddEntry(entry);
	return this;
}
