#ifndef __IMODULESETTINGINFO_H__
#define __IMODULESETTINGINFO_H__
#include <ExodusExtensionInterface/ExodusExtensionInterface.pkg>
#include <InteropSupport/InteropSupport.pkg>
#include <string>

class IModuleSettingInfo
{
public:
	//Constructors
	virtual ~IModuleSettingInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIModuleSettingInfoVersion();
	virtual unsigned int GetIModuleSettingInfoVersion() const = 0;

	//Getters
	inline std::wstring GetName() const;
	inline std::wstring GetDisplayName() const;
	virtual unsigned int GetOptionCount() const = 0;
	virtual unsigned int GetDefaultOptionIndex() const = 0;
	virtual unsigned int GetToggleSettingOnOptionIndex() const = 0;
	virtual unsigned int GetToggleSettingOffOptionIndex() const = 0;
	virtual bool IsToggleSetting() const = 0;

	//Setters
	inline void SetName(const std::wstring& name);
	inline void SetDisplayName(const std::wstring& displayName);
	virtual void SetOptionCount(unsigned int optionCount) = 0;
	virtual void SetDefaultOptionIndex(unsigned int defaultOptionIndex) = 0;
	virtual void SetToggleSettingOnOptionIndex(unsigned int onOptionIndex) = 0;
	virtual void SetToggleSettingOffOptionIndex(unsigned int offOptionIndex) = 0;
	virtual void SetIsToggleSetting(bool toggleSetting) = 0;

protected:
	//Getters
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Setters
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) = 0;
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& displayNameMarshaller) = 0;
};

#include "IModuleSettingInfo.inl"
#endif
