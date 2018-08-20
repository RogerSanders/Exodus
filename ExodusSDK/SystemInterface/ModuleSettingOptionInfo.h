#ifndef __MODULESETTINGOPTIONINFO_H__
#define __MODULESETTINGOPTIONINFO_H__
#include "IModuleSettingOptionInfo.h"

class ModuleSettingOptionInfo :public IModuleSettingOptionInfo
{
public:
	// Constructors
	ModuleSettingOptionInfo();

	// Interface version functions
	virtual unsigned int GetIModuleSettingOptionInfoVersion() const;

	// Getters
	virtual Marshal::Ret<std::wstring> GetName() const;
	virtual Marshal::Ret<std::wstring> GetDisplayName() const;

	// Setters
	virtual void SetName(const Marshal::In<std::wstring>& name);
	virtual void SetDisplayName(const Marshal::In<std::wstring>& displayName);

private:
	std::wstring _name;
	std::wstring _displayName;
};

#endif
