#ifndef __IGENERICACCESSGROUPDATAENTRY_H__
#define __IGENERICACCESSGROUPDATAENTRY_H__
#include "IGenericAccessGroupEntry.h"
#include "IGenericAccess.h"
#include <string>

class IGenericAccessGroupDataEntry :public IGenericAccessGroupEntry
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupDataEntryVersion() { return 1; }
	virtual unsigned int GetIGenericAccessGroupDataEntryVersion() const = 0;

	//Data info functions
	virtual unsigned int GetDataID() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDescription() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetHelpFileLink() const = 0;
};

#endif
