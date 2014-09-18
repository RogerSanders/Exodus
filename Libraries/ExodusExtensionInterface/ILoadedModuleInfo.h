#ifndef __ILOADEDMODULEINFO_H__
#define __ILOADEDMODULEINFO_H__
#include "InteropSupport/InteropSupport.pkg"
#include <string>

class ILoadedModuleInfo
{
public:
	//Constructors
	virtual ~ILoadedModuleInfo() = 0 {}

	//Interface version functions
	static inline unsigned int ThisILoadedModuleInfoVersion();
	virtual unsigned int GetILoadedModuleInfoVersion() const = 0;

	//Internal data
	virtual unsigned int GetModuleID() const = 0;
	virtual void SetModuleID(unsigned int amoduleID) = 0;

	//External information
	inline std::wstring GetModuleFilePath() const;
	inline void SetModuleFilePath(const std::wstring& amoduleFilePath);

	//Required metadata
	virtual bool GetIsProgramModule() const = 0;
	virtual void SetIsProgramModule(bool aprogramModule) = 0;
	inline std::wstring GetSystemClassName() const;
	inline void SetSystemClassName(const std::wstring& asystemClassName);
	inline std::wstring GetClassName() const;
	inline void SetClassName(const std::wstring& aclassName);
	inline std::wstring GetInstanceName() const;
	inline void SetInstanceName(const std::wstring& ainstanceName);
	inline std::wstring GetDisplayName() const;
	inline void SetDisplayName(const std::wstring& adisplayName);

	//Optional metadata
	inline std::wstring GetProductionYear() const;
	inline void SetProductionYear(const std::wstring& aproductionYear);
	inline std::wstring GetManufacturerCode() const;
	inline void SetManufacturerCode(const std::wstring& amanufacturerCode);
	inline std::wstring GetManufacturerDisplayName() const;
	inline void SetManufacturerDisplayName(const std::wstring& amanufacturerDisplayName);

protected:
	//External information
	virtual void GetModuleFilePathInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetModuleFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;

	//Required metadata
	virtual void GetSystemClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetSystemClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetInstanceNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;

	//Optional metadata
	virtual void GetProductionYearInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetProductionYearInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetManufacturerCodeInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetManufacturerCodeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
	virtual void GetManufacturerDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;
	virtual void SetManufacturerDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller) = 0;
};

#include "ILoadedModuleInfo.inl"
#endif
