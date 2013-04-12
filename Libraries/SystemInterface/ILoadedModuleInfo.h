#ifndef __ILOADEDMODULEINFO_H__
#define __ILOADEDMODULEINFO_H__
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
	virtual const wchar_t* GetModuleFilePathInternal() const = 0;
	virtual void SetModuleFilePathInternal(const wchar_t* amoduleFilePath) = 0;

	//Required metadata
	virtual const wchar_t* GetSystemClassNameInternal() const = 0;
	virtual void SetSystemClassNameInternal(const wchar_t* asystemClassName) = 0;
	virtual const wchar_t* GetClassNameInternal() const = 0;
	virtual void SetClassNameInternal(const wchar_t* aclassName) = 0;
	virtual const wchar_t* GetInstanceNameInternal() const = 0;
	virtual void SetInstanceNameInternal(const wchar_t* ainstanceName) = 0;
	virtual const wchar_t* GetDisplayNameInternal() const = 0;
	virtual void SetDisplayNameInternal(const wchar_t* adisplayName) = 0;

	//Optional metadata
	virtual const wchar_t* GetProductionYearInternal() const = 0;
	virtual void SetProductionYearInternal(const wchar_t* aproductionYear) = 0;
	virtual const wchar_t* GetManufacturerCodeInternal() const = 0;
	virtual void SetManufacturerCodeInternal(const wchar_t* amanufacturerCode) = 0;
	virtual const wchar_t* GetManufacturerDisplayNameInternal() const = 0;
	virtual void SetManufacturerDisplayNameInternal(const wchar_t* amanufacturerDisplayName) = 0;
};

#include "ILoadedModuleInfo.inl"
#endif
