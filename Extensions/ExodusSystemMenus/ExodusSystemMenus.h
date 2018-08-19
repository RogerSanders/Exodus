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
	// Constructors
	ExodusSystemMenus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID);
	~ExodusSystemMenus();

	// Window functions
	virtual bool RegisterSystemMenuHandler();
	virtual bool RegisterModuleMenuHandler(unsigned int moduleID);
	virtual void UnregisterModuleMenuHandler(unsigned int moduleID);
	virtual void AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment);
	virtual void AddModuleMenuItems(ModuleMenu moduleMenu, IMenuSegment& menuSegment, unsigned int moduleID);
	virtual bool RestoreSystemViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter);
	virtual bool RestoreModuleViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, unsigned int moduleID);
	virtual bool OpenSystemView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName);
	virtual bool OpenModuleView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, unsigned int moduleID);
	void OpenInputMappingDetailsView(IDevice* targetDevice);
	void CloseInputMappingDetailsView();

private:
	ISystemGUIInterface* _model;
	DebugMenuHandler* _debugMenuHandler;
	SettingsMenuHandler* _settingsMenuHandler;
	SystemMenuHandler* _systemMenuHandler;
	std::map<unsigned int, ModuleOptionMenuHandler*> _moduleOptionMenuHandlers;
};

#endif
