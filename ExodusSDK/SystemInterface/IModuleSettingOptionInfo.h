#ifndef __IMODULESETTINGOPTIONINFO_H__
#define __IMODULESETTINGOPTIONINFO_H__
#include <ExtensionInterface/ExtensionInterface.pkg>
#include "MarshalSupport/MarshalSupport.pkg"
#include <string>
using namespace MarshalSupport::Operators;

class IModuleSettingOptionInfo
{
public:
	//Constructors
	virtual ~IModuleSettingOptionInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIModuleSettingOptionInfoVersion() { return 1; }
	virtual unsigned int GetIModuleSettingOptionInfoVersion() const = 0;

	//Getters
	virtual Marshal::Ret<std::wstring> GetName() const = 0;
	virtual Marshal::Ret<std::wstring> GetDisplayName() const = 0;

	//Setters
	virtual void SetName(const Marshal::In<std::wstring>& name) = 0;
	virtual void SetDisplayName(const Marshal::In<std::wstring>& displayName) = 0;
};

#endif
