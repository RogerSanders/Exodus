#ifndef __MODULESETTINGOPTIONINFO_H__
#define __MODULESETTINGOPTIONINFO_H__
#include "IModuleSettingoptionInfo.h"

class ModuleSettingOptionInfo :public IModuleSettingOptionInfo
{
public:
	//Constructors
	ModuleSettingOptionInfo();

	//Interface version functions
	virtual unsigned int GetIModuleSettingOptionInfoVersion() const;

	//Getters
	std::wstring GetName() const;
	std::wstring GetDisplayName() const;

	//Setters
	void SetName(const std::wstring& aname);
	void SetDisplayName(const std::wstring& adisplayName);

protected:
	//Getters
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Setters
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller);
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller);

private:
	std::wstring name;
	std::wstring displayName;
};

#endif
