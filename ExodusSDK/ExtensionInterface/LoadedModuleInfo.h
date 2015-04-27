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
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleFilePath() const;
	virtual void SetModuleFilePath(const MarshalSupport::Marshal::In<std::wstring>& amoduleFilePath);

	//Required metadata
	virtual bool GetIsProgramModule() const;
	virtual void SetIsProgramModule(bool aprogramModule);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetSystemClassName() const;
	virtual void SetSystemClassName(const MarshalSupport::Marshal::In<std::wstring>& asystemClassName);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleClassName() const;
	virtual void SetModuleClassName(const MarshalSupport::Marshal::In<std::wstring>& aclassName);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleInstanceName() const;
	virtual void SetModuleInstanceName(const MarshalSupport::Marshal::In<std::wstring>& ainstanceName);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetModuleDisplayName() const;
	virtual void SetModuleDisplayName(const MarshalSupport::Marshal::In<std::wstring>& adisplayName);

	//Optional metadata
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetProductionYear() const;
	virtual void SetProductionYear(const MarshalSupport::Marshal::In<std::wstring>& aproductionYear);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetManufacturerCode() const;
	virtual void SetManufacturerCode(const MarshalSupport::Marshal::In<std::wstring>& amanufacturerCode);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetManufacturerDisplayName() const;
	virtual void SetManufacturerDisplayName(const MarshalSupport::Marshal::In<std::wstring>& amanufacturerDisplayName);

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

#endif
