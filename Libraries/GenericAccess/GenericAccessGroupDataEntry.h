#ifndef __GENERICACCESSGROUPDATAENTRY_H__
#define __GENERICACCESSGROUPDATAENTRY_H__
#include "IGenericAccessGroupDataEntry.h"

class GenericAccessGroupDataEntry :public IGenericAccessGroupDataEntry
{
public:
	//Constructors
	template<class T> inline GenericAccessGroupDataEntry(T adataID, const std::wstring& aname);
	inline ~GenericAccessGroupDataEntry();

	//Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupDataEntryVersion() const;

	//Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	//Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	//Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroupDataEntry* SetDataContext(const IGenericAccess::DataContext* adataContext);

	//Data info functions
	virtual unsigned int GetDataID() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	inline GenericAccessGroupDataEntry* SetName(const std::wstring& aname);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDescription() const;
	inline GenericAccessGroupDataEntry* SetDescription(const std::wstring& adescription);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetHelpFileLink() const;
	inline GenericAccessGroupDataEntry* SetHelpFileLink(const std::wstring& ahelpFileLink);

protected:
	//Parent functions
	virtual void SetParent(IGenericAccessGroup* aparent);

private:
	IGenericAccessGroup* parent;
	const IGenericAccess::DataContext* dataContext;
	unsigned int dataID;
	std::wstring name;
	std::wstring description;
	std::wstring helpFileLink;
};

#include "GenericAccessGroupDataEntry.inl"
#endif
