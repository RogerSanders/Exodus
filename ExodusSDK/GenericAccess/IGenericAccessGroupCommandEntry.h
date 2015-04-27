#ifndef __IGENERICACCESSGROUPCOMMANDENTRY_H__
#define __IGENERICACCESSGROUPCOMMANDENTRY_H__
#include "IGenericAccessGroupEntry.h"
#include "IGenericAccess.h"
#include <string>

class IGenericAccessGroupCommandEntry :public IGenericAccessGroupEntry
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupCommandEntryVersion() { return 1; }
	virtual unsigned int GetIGenericAccessGroupCommandEntryVersion() const = 0;

	//Command info functions
	virtual unsigned int GetCommandID() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDescription() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetHelpFileLink() const = 0;
};

#endif
