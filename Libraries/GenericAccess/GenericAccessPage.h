#ifndef __GENERICACCESSPAGE_H__
#define __GENERICACCESSPAGE_H__
#include "IGenericAccessPage.h"
#include "GenericAccessGroup.h"

class GenericAccessPage :public IGenericAccessPage
{
public:
	//Constructors
	GenericAccessPage(const std::wstring& aname);

	//Interface version functions
	virtual unsigned int GetIGenericAccessPageVersion() const;

	//Page info methods
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;

	//Content methods
	virtual const IGenericAccessGroup* GetContentRoot() const;
	GenericAccessPage* AddEntry(IGenericAccessGroupEntry* entry);

private:
	GenericAccessGroup contentRoot;
};

#endif
