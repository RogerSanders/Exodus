#ifndef __GENERICACCESSGROUPSINGLESELECTIONLIST_H__
#define __GENERICACCESSGROUPSINGLESELECTIONLIST_H__
#include "IGenericAccessGroupSingleSelectionList.h"

class GenericAccessGroupSingleSelectionList :public IGenericAccessGroupSingleSelectionList
{
public:
	// Constructors
	template<class T> inline GenericAccessGroupSingleSelectionList(T dataID, const std::wstring& name);
	inline ~GenericAccessGroupSingleSelectionList();

	// Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupSingleSelectionListVersion() const;

	// Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	// Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	// Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroupSingleSelectionList* SetDataContext(const IGenericAccess::DataContext* dataContext);

	// List info functions
	virtual unsigned int GetDataID() const;
	virtual Marshal::Ret<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>> GetSelectionList() const;
	inline GenericAccessGroupSingleSelectionList* SetSelectionList(const std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>& selectionList);
	inline GenericAccessGroupSingleSelectionList* AddSelectionListEntry(const IGenericAccessDataValue* key, const IGenericAccessDataValue* value = 0);
	virtual bool GetAllowNewItemEntry() const;
	inline GenericAccessGroupSingleSelectionList*  SetAllowNewItemEntry(bool state);
	virtual Marshal::Ret<std::wstring> GetName() const;
	inline GenericAccessGroupSingleSelectionList* SetName(const std::wstring& name);
	virtual Marshal::Ret<std::wstring> GetDescription() const;
	inline GenericAccessGroupSingleSelectionList* SetDescription(const std::wstring& description);
	virtual Marshal::Ret<std::wstring> GetHelpFileLink() const;
	inline GenericAccessGroupSingleSelectionList* SetHelpFileLink(const std::wstring& helpFileLink);

protected:
	// Parent functions
	virtual void SetParent(IGenericAccessGroup* parent);

private:
	IGenericAccessGroup* _parent;
	const IGenericAccess::DataContext* _dataContext;
	unsigned int _dataID;
	std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> _selectionList;
	bool _allowNewItemEntry;
	std::wstring _name;
	std::wstring _description;
	std::wstring _helpFileLink;
};

#include "GenericAccessGroupSingleSelectionList.inl"
#endif
