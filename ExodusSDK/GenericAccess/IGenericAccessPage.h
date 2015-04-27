#ifndef __IGENERICACCESSPAGE_H__
#define __IGENERICACCESSPAGE_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include "IGenericAccessGroup.h"
#include <string>

class IGenericAccessPage
{
public:
	//Constructors
	virtual ~IGenericAccessPage() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGenericAccessPageVersion() { return 1; }
	virtual unsigned int GetIGenericAccessPageVersion() const = 0;

	//Page info functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetTitle() const = 0;

	//Content functions
	virtual const IGenericAccessGroup* GetContentRoot() const = 0;
};

#endif
