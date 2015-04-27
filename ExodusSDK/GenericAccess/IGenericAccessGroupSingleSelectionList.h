#ifndef __IGENERICACCESSGROUPSINGLESELECTIONLIST_H__
#define __IGENERICACCESSGROUPSINGLESELECTIONLIST_H__
#include "IGenericAccessGroupEntry.h"
#include <string>
#include <list>
#include <utility>

class IGenericAccessGroupSingleSelectionList :public IGenericAccessGroupEntry
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupSingleSelectionListVersion() { return 1; }
	virtual unsigned int GetIGenericAccessGroupSingleSelectionListVersion() const = 0;

	//List info functions
	virtual unsigned int GetDataID() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>> GetSelectionList() const = 0;
	virtual bool GetAllowNewItemEntry() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDescription() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetHelpFileLink() const = 0;
};

#endif
