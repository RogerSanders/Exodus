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
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDisplayName() const;

	//Setters
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& aname);
	virtual void SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName);

private:
	std::wstring name;
	std::wstring displayName;
};

#endif
