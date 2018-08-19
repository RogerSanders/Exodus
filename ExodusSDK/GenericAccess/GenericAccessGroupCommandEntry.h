#ifndef __GENERICACCESSGROUPCOMMANDENTRY_H__
#define __GENERICACCESSGROUPCOMMANDENTRY_H__
#include "IGenericAccessGroupCommandEntry.h"

class GenericAccessGroupCommandEntry :public IGenericAccessGroupCommandEntry
{
public:
	// Constructors
	template<class T> inline GenericAccessGroupCommandEntry(T commandID, const std::wstring& name);
	inline ~GenericAccessGroupCommandEntry();

	// Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupCommandEntryVersion() const;

	// Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	// Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	// Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroupCommandEntry* SetDataContext(const IGenericAccess::DataContext* dataContext);

	// Command info methods
	virtual unsigned int GetCommandID() const;
	virtual Marshal::Ret<std::wstring> GetName() const;
	inline GenericAccessGroupCommandEntry* SetName(const std::wstring& name);
	virtual Marshal::Ret<std::wstring> GetDescription() const;
	inline GenericAccessGroupCommandEntry* SetDescription(const std::wstring& description);
	virtual Marshal::Ret<std::wstring> GetHelpFileLink() const;
	inline GenericAccessGroupCommandEntry* SetHelpFileLink(const std::wstring& helpFileLink);

protected:
	// Parent functions
	virtual void SetParent(IGenericAccessGroup* parent);

private:
	IGenericAccessGroup* _parent;
	const IGenericAccess::DataContext* _dataContext;
	unsigned int _commandID;
	std::wstring _name;
	std::wstring _description;
	std::wstring _helpFileLink;
};

#include "GenericAccessGroupCommandEntry.inl"
#endif
