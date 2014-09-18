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
	static inline unsigned int ThisIGenericAccessGroupSingleSelectionListVersion();
	virtual unsigned int GetIGenericAccessGroupSingleSelectionListVersion() const = 0;

	//List info functions
	virtual unsigned int GetDataID() const = 0;
	inline std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> GetSelectionList() const;
	virtual bool GetAllowNewItemEntry() const = 0;
	inline std::wstring GetName() const;
	inline std::wstring GetDescription() const;
	inline std::wstring GetHelpFileLink() const;

protected:
	//List info functions
	virtual void GetSelectionListInternal(const InteropSupport::ISTLObjectTarget<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>>& marshaller) const = 0;
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDescriptionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetHelpFileLinkInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IGenericAccessGroupSingleSelectionList.inl"
#endif
