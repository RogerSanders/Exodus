#ifndef __IGENERICACCESSGROUPENTRY_H__
#define __IGENERICACCESSGROUPENTRY_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include "IGenericAccess.h"
#include <string>
class IGenericAccessGroup;

class IGenericAccessGroupEntry
{
public:
	//Enumerations
	enum class GroupEntryType;

public:
	//Constructors
	virtual ~IGenericAccessGroupEntry() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupEntryVersion() { return 1; }
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const = 0;

	//Type functions
	virtual GroupEntryType GetGroupEntryType() const = 0;
	virtual bool IsGroup() const = 0;

	//Parent functions
	virtual IGenericAccessGroup* GetParent() const = 0;

	//Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const = 0;

protected:
	//Friend declarations
	friend class IGenericAccessGroup;

	//Parent functions
	virtual void SetParent(IGenericAccessGroup* aparent) = 0;
};

#include "IGenericAccessGroupEntry.inl"
#endif
