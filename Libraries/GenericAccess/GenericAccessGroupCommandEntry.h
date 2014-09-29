#ifndef __GENERICACCESSGROUPCOMMANDENTRY_H__
#define __GENERICACCESSGROUPCOMMANDENTRY_H__
#include "IGenericAccessGroupCommandEntry.h"

class GenericAccessGroupCommandEntry :public IGenericAccessGroupCommandEntry
{
public:
	//Constructors
	template<class T> inline GenericAccessGroupCommandEntry(T acommandID, const std::wstring& aname);
	inline ~GenericAccessGroupCommandEntry();

	//Interface version functions
	virtual unsigned int GetIGenericAccessGroupEntryVersion() const;
	virtual unsigned int GetIGenericAccessGroupCommandEntryVersion() const;

	//Type functions
	virtual GroupEntryType GetGroupEntryType() const;
	virtual bool IsGroup() const;

	//Parent functions
	virtual IGenericAccessGroup* GetParent() const;

	//Data context functions
	virtual const IGenericAccess::DataContext* GetDataContext() const;
	inline GenericAccessGroupCommandEntry* SetDataContext(const IGenericAccess::DataContext* adataContext);

	//Command info methods
	virtual unsigned int GetCommandID() const;
	inline std::wstring GetName() const;
	inline GenericAccessGroupCommandEntry* SetName(const std::wstring& aname);
	inline std::wstring GetDescription() const;
	inline GenericAccessGroupCommandEntry* SetDescription(const std::wstring& adescription);
	inline std::wstring GetHelpFileLink() const;
	inline GenericAccessGroupCommandEntry* SetHelpFileLink(const std::wstring& ahelpFileLink);

protected:
	//Parent functions
	virtual void SetParent(IGenericAccessGroup* aparent);

	//Command info functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDescriptionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetHelpFileLinkInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	IGenericAccessGroup* parent;
	const IGenericAccess::DataContext* dataContext;
	unsigned int commandID;
	std::wstring name;
	std::wstring description;
	std::wstring helpFileLink;
};

#include "GenericAccessGroupCommandEntry.inl"
#endif
