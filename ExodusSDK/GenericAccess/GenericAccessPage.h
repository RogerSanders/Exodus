#ifndef __GENERICACCESSPAGE_H__
#define __GENERICACCESSPAGE_H__
#include "IGenericAccessPage.h"
#include "GenericAccessGroup.h"

class GenericAccessPage :public IGenericAccessPage
{
public:
	//Constructors
	GenericAccessPage(const std::wstring& aname, const std::wstring& atitle = L"", Type atype = Type::Debug);

	//Interface version functions
	virtual unsigned int GetIGenericAccessPageVersion() const;

	//Page info methods
	virtual Type GetPageType() const;
	void SetPageType(Type atype);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetTitle() const;

	//Content methods
	virtual const IGenericAccessGroup* GetContentRoot() const;
	GenericAccessPage* AddEntry(IGenericAccessGroupEntry* entry);

private:
	GenericAccessGroup contentRoot;
	Type type;
	std::wstring title;
};

#endif
