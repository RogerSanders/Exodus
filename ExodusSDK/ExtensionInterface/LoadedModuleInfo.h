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
	virtual void SetModuleID(unsigned int moduleID);

	//External information
	virtual Marshal::Ret<std::wstring> GetModuleFilePath() const;
	virtual void SetModuleFilePath(const Marshal::In<std::wstring>& moduleFilePath);

	//Required metadata
	virtual bool GetIsProgramModule() const;
	virtual void SetIsProgramModule(bool programModule);
	virtual Marshal::Ret<std::wstring> GetSystemClassName() const;
	virtual void SetSystemClassName(const Marshal::In<std::wstring>& systemClassName);
	virtual Marshal::Ret<std::wstring> GetModuleClassName() const;
	virtual void SetModuleClassName(const Marshal::In<std::wstring>& className);
	virtual Marshal::Ret<std::wstring> GetModuleInstanceName() const;
	virtual void SetModuleInstanceName(const Marshal::In<std::wstring>& instanceName);
	virtual Marshal::Ret<std::wstring> GetModuleDisplayName() const;
	virtual void SetModuleDisplayName(const Marshal::In<std::wstring>& displayName);

	//Optional metadata
	virtual Marshal::Ret<std::wstring> GetProductionYear() const;
	virtual void SetProductionYear(const Marshal::In<std::wstring>& productionYear);
	virtual Marshal::Ret<std::wstring> GetManufacturerCode() const;
	virtual void SetManufacturerCode(const Marshal::In<std::wstring>& manufacturerCode);
	virtual Marshal::Ret<std::wstring> GetManufacturerDisplayName() const;
	virtual void SetManufacturerDisplayName(const Marshal::In<std::wstring>& manufacturerDisplayName);

private:
	//Internal data
	unsigned int _moduleID;

	//External information
	std::wstring _filePath;

	//Required metadata
	bool _programModule;
	std::wstring _systemClassName;
	std::wstring _className;
	std::wstring _instanceName;
	std::wstring _displayName;
	//##TODO## Consider whether this should be implemented
	//unsigned int versionNumber;

	//Optional metadata
	std::wstring _productionYear;
	std::wstring _manufacturerCode;
	std::wstring _manufacturerDisplayName;
};

#endif
