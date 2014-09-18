#ifndef __IGENERICACCESSGROUPCOLLECTIONENTRY_H__
#define __IGENERICACCESSGROUPCOLLECTIONENTRY_H__
#include "IGenericAccessGroup.h"
#include "IGenericAccessGroupEntry.h"
#include "IGenericAccessDataValue.h"
#include <list>

class IGenericAccessGroupCollectionEntry :public IGenericAccessGroup
{
public:
	//Structures
	struct CollectionEntry;

public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupCollectionEntryVersion();
	virtual unsigned int GetIGenericAccessGroupCollectionEntryVersion() const = 0;

	//Collection entry functions
	virtual IGenericAccessDataValue::DataType GetKeyDataType() const = 0;
	inline std::list<CollectionEntry> GetCollectionEntries() const;
	inline IGenericAccessGroupEntry* GetCollectionEntry(const std::wstring& key) const;
	virtual IGenericAccessGroupEntry* GetCollectionEntry(const IGenericAccessDataValue& key) const = 0;

	//Modification functions
	virtual unsigned int GetLastModifiedToken() const = 0;
	virtual void ObtainReadLock() const = 0;
	virtual void ReleaseReadLock() const = 0;
	virtual void ObtainWriteLock() const = 0;
	virtual void ReleaseWriteLock() const = 0;

protected:
	//Collection entry functions
	virtual void GetCollectionEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<CollectionEntry>>& marshaller) const = 0;
	virtual IGenericAccessGroupEntry* GetCollectionEntryInternal(const InteropSupport::ISTLObjectSource<std::wstring>& keyMarshaller) const = 0;
};

#include "IGenericAccessGroupCollectionEntry.inl"
#endif
