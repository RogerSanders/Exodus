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
	inline std::wstring GetName() const;
	inline GenericAccessGroupCollectionEntry* SetName(const std::wstring& aname);
	virtual bool GetOpenByDefault() const;
	inline GenericAccessGroupCollectionEntry* SetOpenByDefault(bool state);

	//Collection entry functions
	virtual IGenericAccessDataValue::DataType GetKeyDataType() const;
	inline GenericAccessGroupCollectionEntry* SetKeyDataType(IGenericAccessDataValue::DataType akeyDataType);
	virtual unsigned int GetEntryCount() const;
	inline std::list<IGenericAccessGroupEntry*> GetEntries() const;
	inline std::list<CollectionEntry> GetCollectionEntries() const;
	inline GenericAccessGroupCollectionEntry* AddCollectionEntry(IGenericAccessDataValue* key, IGenericAccessGroupEntry* value);
	inline GenericAccessGroupCollectionEntry* AddCollectionEntry(const CollectionEntry& entry);
	inline GenericAccessGroupCollectionEntry* RemoveCollectionEntry(const std::wstring& key);
	inline GenericAccessGroupCollectionEntry* RemoveCollectionEntry(const IGenericAccessDataValue& key);
	inline GenericAccessGroupCollectionEntry* RemoveCollectionEntry(IGenericAccessGroupEntry* value);
	IGenericAccessGroupEntry* GetCollectionEntry(const std::wstring& key) const;
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

	//Group info methods
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Collection entry functions
	virtual void GetEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<IGenericAccessGroupEntry*>>& marshaller) const;
	virtual void GetCollectionEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<CollectionEntry>>& marshaller) const;
	virtual IGenericAccessGroupEntry* GetCollectionEntryInternal(const InteropSupport::ISTLObjectSource<std::wstring>& keyMarshaller) const;

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
