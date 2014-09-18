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
	std::wstring GetName() const;

	//Content methods
	virtual const IGenericAccessGroup* GetContentRoot() const;
	GenericAccessPage* AddEntry(IGenericAccessGroupEntry* entry);

protected:
	//Page info methods
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	GenericAccessGroup contentRoot;
};

#endif
