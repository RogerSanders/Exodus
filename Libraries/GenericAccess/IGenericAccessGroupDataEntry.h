#ifndef __IGENERICACCESSGROUPDATAENTRY_H__
#define __IGENERICACCESSGROUPDATAENTRY_H__
#include "IGenericAccessGroupEntry.h"
#include "IGenericAccess.h"
#include <string>

class IGenericAccessGroupDataEntry :public IGenericAccessGroupEntry
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupDataEntryVersion();
	virtual unsigned int GetIGenericAccessGroupDataEntryVersion() const = 0;

	//Data info functions
	virtual unsigned int GetDataID() const = 0;
	inline std::wstring GetName() const;
	inline std::wstring GetDescription() const;
	inline std::wstring GetHelpFileLink() const;

protected:
	//Data info functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDescriptionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetHelpFileLinkInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IGenericAccessGroupDataEntry.inl"
#endif
