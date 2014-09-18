#ifndef __IGENERICACCESSGROUPCOMMANDENTRY_H__
#define __IGENERICACCESSGROUPCOMMANDENTRY_H__
#include "IGenericAccessGroupEntry.h"
#include "IGenericAccess.h"
#include <string>

class IGenericAccessGroupCommandEntry :public IGenericAccessGroupEntry
{
public:
	//Interface version functions
	static inline unsigned int ThisIGenericAccessGroupCommandEntryVersion();
	virtual unsigned int GetIGenericAccessGroupCommandEntryVersion() const = 0;

	//Command info functions
	virtual unsigned int GetCommandID() const = 0;
	inline std::wstring GetName() const;
	inline std::wstring GetDescription() const;
	inline std::wstring GetHelpFileLink() const;

protected:
	//Command info functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDescriptionInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetHelpFileLinkInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
};

#include "IGenericAccessGroupCommandEntry.inl"
#endif
