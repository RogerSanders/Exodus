#ifndef __IGENERICACCESSGROUP_H__
#define __IGENERICACCESSGROUP_H__
#include "IGenericAccessGroupEntry.h"
#include "InteropSupport/InteropSupport.pkg"
#include <string>
#include <list>

class IGenericAccessGroup :public IGenericAccessGroupEntry
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupVersion();
	virtual unsigned int GetIGenericAccessGroupVersion() const = 0;

	//Group info functions
	inline std::wstring GetName() const;
	virtual bool GetOpenByDefault() const = 0;

	//Entry functions
	virtual unsigned int GetEntryCount() const = 0;
	inline std::list<IGenericAccessGroupEntry*> GetEntries() const;

protected:
	//Parent functions
	inline void SetParentForTargetEntry(IGenericAccessGroupEntry* entry, IGenericAccessGroup* aparent) const;

	//Group info functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Entry functions
	virtual void GetEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<IGenericAccessGroupEntry*>>& marshaller) const = 0;
};

#include "IGenericAccessGroup.inl"
#endif
