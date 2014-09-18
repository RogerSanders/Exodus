#ifndef __EXODUSSYSTEMMENUS_H__
#define __EXODUSSYSTEMMENUS_H__
#include "Extension/Extension.pkg"
#include <map>
#include "SystemInterface/SystemInterface.pkg"
class DebugMenuHandler;
class SettingsMenuHandler;
class SystemMenuHandler;
class ModuleOptionMenuHandler;

class ExodusSystemMenus :public Extension
{
public:
	//Constructors
	ExodusSystemMenus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID);
	~ExodusSystemMenus();

	//Window functions
	virtual bool RegisterSystemMenuHandler();
	virtual bool RegisterModuleMenuHandler(unsigned int moduleID);
	virtual void UnregisterModuleMenuHandler(unsigned int moduleID);
	virtual void AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment);
	virtual void AddModuleMenuItems(ModuleMenu moduleMenu, IMenuSegment& menuSegment, unsigned int moduleID);
	virtual bool RestoreSystemViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState);
	virtual bool RestoreModuleViewState(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID);
	virtual bool OpenSystemView(const std::wstring& viewGroupName, const std::wstring& viewName);
	virtual bool OpenModuleView(const std::wstring& viewGroupName, const std::wstring& viewName, unsigned int moduleID);
	void OpenLoggerDetailsView(const ISystemGUIInterface::SystemLogEntry& alogEntry);
	void OpenInputMappingDetailsView(IDevice* targetDevice);
	void CloseInputMappingDetailsView();

private:
	ISystemGUIInterface* model;
	DebugMenuHandler* debugMenuHandler;
	SettingsMenuHandler* settingsMenuHandler;
	SystemMenuHandler* systemMenuHandler;
	std::map<unsigned int, ModuleOptionMenuHandler*> moduleOptionMenuHandlers;
};

#endif
