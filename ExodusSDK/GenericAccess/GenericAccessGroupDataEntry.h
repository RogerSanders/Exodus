#ifndef __GENERICACCESSGROUPDATAENTRY_H__
#define __GENERICACCESSGROUPDATAENTRY_H__
#include "IGenericAccessGroupDataEntry.h"

class GenericAccessGroupDataEntry :public IGenericAccessGroupDataEntry
{
public:
	// Constructors
	template<class T>
	inline GenericAccessGroupDataEntry(T dataID, const std::wstring& name);
	inline ~GenericAccessGroupDataEntry();

	// Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupDataEntryVersion() const;

	// Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	// Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	// Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroupDataEntry* SetDataContext(const IGenericAccess::DataContext* dataContext);

	// Data info functions
	virtual unsigned int GetDataID() const;
	virtual Marshal::Ret<std::wstring> GetName() const;
	inline GenericAccessGroupDataEntry* SetName(const std::wstring& name);
	virtual Marshal::Ret<std::wstring> GetDescription() const;
	inline GenericAccessGroupDataEntry* SetDescription(const std::wstring& description);
	virtual Marshal::Ret<std::wstring> GetHelpFileLink() const;
	inline GenericAccessGroupDataEntry* SetHelpFileLink(const std::wstring& helpFileLink);

protected:
	// Parent functions
	virtual void SetParent(IGenericAccessGroup* parent);

private:
	IGenericAccessGroup* _parent;
	const IGenericAccess::DataContext* _dataContext;
	unsigned int _dataID;
	std::wstring _name;
	std::wstring _description;
	std::wstring _helpFileLink;
};

#include "GenericAccessGroupDataEntry.inl"
#endif
