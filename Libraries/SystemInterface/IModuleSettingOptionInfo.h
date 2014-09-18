#ifndef __IMODULESETTINGOPTIONINFO_H__
#define __IMODULESETTINGOPTIONINFO_H__
#include <ExodusExtensionInterface/ExodusExtensionInterface.pkg>
#include <InteropSupport/InteropSupport.pkg>
#include <string>

class IModuleSettingOptionInfo
{
public:
	//Constructors
	virtual ~IModuleSettingOptionInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIModuleSettingOptionInfoVersion();
	virtual unsigned int GetIModuleSettingOptionInfoVersion() const = 0;

	//Getters
	inline std::wstring GetName() const;
	inline std::wstring GetDisplayName() const;

	//Setters
	inline void SetName(const std::wstring& name);
	inline void SetDisplayName(const std::wstring& displayName);

protected:
	//Getters
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Setters
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) = 0;
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller) = 0;
};

#include "IModuleSettingOptionInfo.inl"
#endif
