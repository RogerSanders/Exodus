#ifndef __GENERICACCESSGROUPSINGLESELECTIONLIST_H__
#define __GENERICACCESSGROUPSINGLESELECTIONLIST_H__
#include "IGenericAccessGroupSingleSelectionList.h"

class GenericAccessGroupSingleSelectionList :public IGenericAccessGroupSingleSelectionList
{
public:
	//Constructors
	inline GenericAccessGroupSingleSelectionList(unsigned int acommandID, const std::wstring& aname);
	inline ~GenericAccessGroupSingleSelectionList();

	//Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupSingleSelectionListVersion() const;

	//Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	//Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	//Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroupSingleSelectionList* SetDataContext(const IGenericAccess::DataContext* adataContext);

	//List info functions
	virtual unsigned int GetDataID() const;
	inline std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> GetSelectionList() const;
	inline GenericAccessGroupSingleSelectionList* SetSelectionList(const std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>& aselectionList);
	inline GenericAccessGroupSingleSelectionList* AddSelectionListEntry(const IGenericAccessDataValue* key, const IGenericAccessDataValue* value = 0);
	virtual bool GetAllowNewItemEntry() const;
	inline GenericAccessGroupSingleSelectionList*  SetAllowNewItemEntry(bool state);
	inline std::wstring GetName() const;
	inline GenericAccessGroupSingleSelectionList* SetName(const std::wstring& aname);
	inline std::wstring GetDescription() const;
	inline GenericAccessGroupSingleSelectionList* SetDescription(const std::wstring& adescription);
	inline std::wstring GetHelpFileLink() const;
	inline GenericAccessGroupSingleSelectionList* SetHelpFileLink(const std::wstring& ahelpFileLink);

protected:
	//Parent functions
	virtual void SetParent(IGenericAccessGroup* aparent);

	//List info functions
	virtual void GetSelectionListInternal(const InteropSupport::ISTLObjectTarget<std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>>>& marshaller) const;
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDescriptionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetHelpFileLinkInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	IGenericAccessGroup* parent;
	const IGenericAccess::DataContext* dataContext;
	unsigned int dataID;
	std::list<std::pair<const IGenericAccessDataValue*, const IGenericAccessDataValue*>> selectionList;
	bool allowNewItemEntry;
	std::wstring name;
	std::wstring description;
	std::wstring helpFileLink;
};

#include "GenericAccessGroupSingleSelectionList.inl"
#endif
