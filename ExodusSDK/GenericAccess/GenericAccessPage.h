#ifndef __GENERICACCESSPAGE_H__
#define __GENERICACCESSPAGE_H__
#include "IGenericAccessPage.h"
#include "GenericAccessGroup.h"

class GenericAccessPage :public IGenericAccessPage
{
public:
	//Constructors
	GenericAccessPage(const std::wstring& name, const std::wstring& title = L"", Type type = Type::Debug);

	//Interface version functions
	virtual unsigned int GetIGenericAccessPageVersion() const;

	//Page info methods
	virtual Type GetPageType() const;
	void SetPageType(Type type);
	virtual Marshal::Ret<std::wstring> GetName() const;
	virtual Marshal::Ret<std::wstring> GetTitle() const;

	//Content methods
	virtual const IGenericAccessGroup* GetContentRoot() const;
	GenericAccessPage* AddEntry(IGenericAccessGroupEntry* entry);

private:
	GenericAccessGroup _contentRoot;
	Type _type;
	std::wstring _title;
};

#endif
