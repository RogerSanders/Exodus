#ifndef __LOADEDMODULEINFO_H__
#define __LOADEDMODULEINFO_H__
#include "ILoadedModuleInfo.h"

class LoadedModuleInfo :public ILoadedModuleInfo
{
public:
	//Constructors
	LoadedModuleInfo();

	//Interface version functions
	virtual unsigned int GetILoadedModuleInfoVersion() const;

	//Internal data
	virtual unsigned int GetModuleID() const;
	virtual void SetModuleID(unsigned int amoduleID);

	//External information
	inline std::wstring GetModuleFilePath() const;
	inline void SetModuleFilePath(const std::wstring& amoduleFilePath);

	//Required metadata
	virtual bool GetIsProgramModule() const;
	virtual void SetIsProgramModule(bool aprogramModule);
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
	virtual void GetModuleFilePathInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetModuleFilePathInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

	//Required metadata
	virtual void GetSystemClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetSystemClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetClassNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetClassNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetInstanceNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

	//Optional metadata
	virtual void GetProductionYearInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetProductionYearInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetManufacturerCodeInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetManufacturerCodeInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);
	virtual void GetManufacturerDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetManufacturerDisplayNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

private:
	//Internal data
	unsigned int moduleID;

	//External information
	std::wstring filePath;

	//Required metadata
	bool programModule;
	std::wstring systemClassName;
	std::wstring className;
	std::wstring instanceName;
	std::wstring displayName;
	//##TODO## Consider whether this should be implemented
	//unsigned int versionNumber;

	//Optional metadata
	std::wstring productionYear;
	std::wstring manufacturerCode;
	std::wstring manufacturerDisplayName;
};

#include "LoadedModuleInfo.inl"
#endif
