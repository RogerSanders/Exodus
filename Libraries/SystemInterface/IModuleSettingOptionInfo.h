#ifndef __IMODULESETTINGOPTIONINFO_H__
#define __IMODULESETTINGOPTIONINFO_H__
#include <ExodusExtensionInterface/ExodusExtensionInterface.pkg>
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>

class IModuleSettingOptionInfo
{
public:
	//Constructors
	virtual ~IModuleSettingOptionInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIModuleSettingOptionInfoVersion() { return 1; }
	virtual unsigned int GetIModuleSettingOptionInfoVersion() const = 0;

	//Getters
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetName() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetDisplayName() const = 0;

	//Setters
	virtual void SetName(const MarshalSupport::Marshal::In<std::wstring>& name) = 0;
	virtual void SetDisplayName(const MarshalSupport::Marshal::In<std::wstring>& displayName) = 0;
};

#endif
