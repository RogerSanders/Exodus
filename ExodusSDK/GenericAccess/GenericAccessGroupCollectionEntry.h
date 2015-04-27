#ifndef __GENERICACCESSGROUPCOLLECTIONENTRY_H__
#define __GENERICACCESSGROUPCOLLECTIONENTRY_H__
#include "IGenericAccessGroupCollectionEntry.h"
#include "ThreadLib/ThreadLib.pkg"

class GenericAccessGroupCollectionEntry :public IGenericAccessGroupCollectionEntry
{
public:
	//Constructors
	inline GenericAccessGroupCollectionEntry(const std::wstring& aname, IGenericAccessDataValue::DataType akeyDataType);
	inline ~GenericAccessGroupCollectionEntry();

	//Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupVersion() const;
	virtual unsigned int GetIGenericAccessGroupCollectionEntryVersion() const;

	//Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	//Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	//Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroupCollectionEntry* SetDataContext(const IGenericAccess::DataContext* adataContext);

	//Group info functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	inline GenericAccessGroupCollectionEntry* SetName(const std::wstring& aname);
	virtual bool GetOpenByDefault() const;
	inline GenericAccessGroupCollectionEntry* SetOpenByDefault(bool state);

	//Collection entry functions
	virtual IGenericAccessDataValue::DataType GetKeyDataType() const;
	inline GenericAccessGroupCollectionEntry* SetKeyDataType(IGenericAccessDataValue::DataType akeyDataType);
	virtual unsigned int GetEntryCount() const;
	virtual MarshalSupport::Marshal::Ret<std::list<IGenericAccessGroupEntry*>> GetEntries() const;
	virtual MarshalSupport::Marshal::Ret<std::list<CollectionEntry>> GetCollectionEntries() const;
	inline GenericAccessGroupCollectionEntry* AddCollectionEntry(IGenericAccessDataValue* key, IGenericAccessGroupEntry* value);
	inline GenericAccessGroupCollectionEntry* AddCollectionEntry(const CollectionEntry& entry);
	inline GenericAccessGroupCollectionEntry* RemoveCollectionEntry(const std::wstring& key);
	inline GenericAccessGroupCollectionEntry* RemoveCollectionEntry(const IGenericAccessDataValue& key);
	inline GenericAccessGroupCollectionEntry* RemoveCollectionEntry(IGenericAccessGroupEntry* value);
	virtual IGenericAccessGroupEntry* GetCollectionEntry(const MarshalSupport::Marshal::In<std::wstring>& key) const;
	virtual IGenericAccessGroupEntry* GetCollectionEntry(const IGenericAccessDataValue& key) const;

	//Modification functions
	virtual unsigned int GetLastModifiedToken() const;
	virtual void ObtainReadLock() const;
	virtual void ReleaseReadLock() const;
	virtual void ObtainWriteLock() const;
	virtual void ReleaseWriteLock() const;

protected:
	//Parent functions
	virtual void SetParent(IGenericAccessGroup* aparent);

private:
	IGenericAccessGroup* parent;
	const IGenericAccess::DataContext* dataContext;
	std::wstring name;
	bool openByDefault;
	unsigned int lastModifiedToken;
	IGenericAccessDataValue::DataType keyDataType;
	std::list<CollectionEntry> entries;
	mutable ReadWriteLock readWriteLock;
};

#include "GenericAccessGroupCollectionEntry.inl"
#endif
