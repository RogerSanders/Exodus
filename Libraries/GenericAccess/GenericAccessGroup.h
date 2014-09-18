#ifndef __GENERICACCESSGROUP_H__
#define __GENERICACCESSGROUP_H__
#include "IGenericAccessGroup.h"
#include <string>
#include <list>

class GenericAccessGroup :public IGenericAccessGroup
{
public:
	//Constructors
	inline GenericAccessGroup(const std::wstring& aname);
	inline ~GenericAccessGroup();

	//Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupVersion() const;

	//Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	//Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	//Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroup* SetDataContext(const IGenericAccess::DataContext* adataContext);

	//Group info methods
	inline std::wstring GetName() const;
	inline GenericAccessGroup* SetName(const std::wstring& aname);
	virtual bool GetOpenByDefault() const;
	inline GenericAccessGroup* SetOpenByDefault(bool state);

	//Entry methods
	virtual unsigned int GetEntryCount() const;
	inline std::list<IGenericAccessGroupEntry*> GetEntries() const;
	inline GenericAccessGroup* AddEntry(IGenericAccessGroupEntry* entry);
	inline GenericAccessGroup* RemoveEntry(IGenericAccessGroupEntry* entry);

protected:
	//Parent functions
	virtual void SetParent(IGenericAccessGroup* aparent);

	//Group info methods
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Entry methods
	virtual void GetEntriesInternal(const InteropSupport::ISTLObjectTarget<std::list<IGenericAccessGroupEntry*>>& marshaller) const;

private:
	IGenericAccessGroup* parent;
	const IGenericAccess::DataContext* dataContext;
	std::wstring name;
	bool openByDefault;
	std::list<IGenericAccessGroupEntry*> childEntries;
};

#include "GenericAccessGroup.inl"
#endif
