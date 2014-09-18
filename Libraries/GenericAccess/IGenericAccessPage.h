#ifndef __IGENERICACCESSPAGE_H__
#define __IGENERICACCESSPAGE_H__
#include "InteropSupport/InteropSupport.pkg"
#include "IGenericAccessGroup.h"
#include <string>

class IGenericAccessPage
{
public:
	//Constructors
	virtual ~IGenericAccessPage() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGenericAccessPageVersion();
	virtual unsigned int GetIGenericAccessPageVersion() const = 0;

	//Page info functions
	inline std::wstring GetName() const;

	//Content functions
	virtual const IGenericAccessGroup* GetContentRoot() const = 0;

protected:
	//Page info functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IGenericAccessPage.inl"
#endif
