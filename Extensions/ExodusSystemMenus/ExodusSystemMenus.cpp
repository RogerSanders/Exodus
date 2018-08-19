#include "ExodusSystemMenus.h"
#include "DebugMenuHandler.h"
#include "SettingsMenuHandler.h"
#include "SystemMenuHandler.h"
#include "ModuleOptionMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExodusSystemMenus::ExodusSystemMenus(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:Extension(implementationName, instanceName, moduleID), _model(0), _debugMenuHandler(0), _settingsMenuHandler(0), _systemMenuHandler(0)
{}

//----------------------------------------------------------------------------------------
ExodusSystemMenus::~ExodusSystemMenus()
{
	//Delete all menu handlers
	if (_debugMenuHandler != 0)
	{
		_debugMenuHandler->ClearMenuItems();
		delete _debugMenuHandler;
	}
	if (_settingsMenuHandler != 0)
	{
		_settingsMenuHandler->ClearMenuItems();
		delete _settingsMenuHandler;
	}
	if (_systemMenuHandler != 0)
	{
		_systemMenuHandler->ClearMenuItems();
		delete _systemMenuHandler;
	}
	for (std::map<unsigned int, ModuleOptionMenuHandler*>::const_iterator i = _moduleOptionMenuHandlers.begin(); i != _moduleOptionMenuHandlers.end(); ++i)
	{
		ModuleOptionMenuHandler* entry = i->second;
		delete entry;
	}
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::RegisterSystemMenuHandler()
{
	//Retrieve the system GUI interface
	_model = &((ISystemGUIInterface&)GetSystemInterface());

	//Create all menu handlers
	_debugMenuHandler = new DebugMenuHandler(*this, *_model);
	_debugMenuHandler->LoadMenuItems();
	_settingsMenuHandler = new SettingsMenuHandler(*this, *_model);
	_settingsMenuHandler->LoadMenuItems();
	_systemMenuHandler = new SystemMenuHandler(*this, *_model);
	_systemMenuHandler->LoadMenuItems();

	return true;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::RegisterModuleMenuHandler(unsigned int moduleID)
{
	ModuleOptionMenuHandler* menuHandler = new ModuleOptionMenuHandler(*this, *_model, moduleID);
	_moduleOptionMenuHandlers.insert(std::pair<unsigned int, ModuleOptionMenuHandler*>(moduleID, menuHandler));
	return true;
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::UnregisterModuleMenuHandler(unsigned int moduleID)
{
	std::map<unsigned int, ModuleOptionMenuHandler*>::iterator moduleOptionMenuHandlerIterator = _moduleOptionMenuHandlers.find(moduleID);
	if (moduleOptionMenuHandlerIterator != _moduleOptionMenuHandlers.end())
	{
		ModuleOptionMenuHandler* entry = moduleOptionMenuHandlerIterator->second;
		delete entry;
		_moduleOptionMenuHandlers.erase(moduleOptionMenuHandlerIterator);
	}
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment)
{
	switch (systemMenu)
	{
	case IExtension::SystemMenu::Debug:
		_debugMenuHandler->AddMenuItems(menuSegment);
		break;
	case IExtension::SystemMenu::Settings:
		_settingsMenuHandler->AddMenuItems(menuSegment);
		break;
	case IExtension::SystemMenu::System:
		_systemMenuHandler->AddMenuItems(menuSegment);
		break;
	}
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::AddModuleMenuItems(ModuleMenu moduleMenu, IMenuSegment& menuSegment, unsigned int moduleID)
{
	if (moduleMenu == IExtension::ModuleMenu::Settings)
	{
		ModuleOptionMenuHandler& menuHandler = *_moduleOptionMenuHandlers[moduleID];
		menuHandler.AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::RestoreSystemViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter)
{
	bool result = false;
	result |= _debugMenuHandler->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	result |= _settingsMenuHandler->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	result |= _systemMenuHandler->RestoreMenuViewOpen(viewGroupName, viewName, viewState, restoredViewPresenter);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::RestoreModuleViewState(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, unsigned int moduleID)
{
	//Nothing to do here, since our current module option menu handler has no views.
	return false;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::OpenSystemView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName)
{
	bool result = false;
	result |= _debugMenuHandler->OpenView(viewGroupName, viewName);
	result |= _settingsMenuHandler->OpenView(viewGroupName, viewName);
	result |= _systemMenuHandler->OpenView(viewGroupName, viewName);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::OpenModuleView(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, unsigned int moduleID)
{
	//Nothing to do here, since our current module option menu handler has no views.
	return false;
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::OpenInputMappingDetailsView(IDevice* targetDevice)
{
	_settingsMenuHandler->OpenInputMappingDetailsView(targetDevice);
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::CloseInputMappingDetailsView()
{
	_settingsMenuHandler->CloseInputMappingDetailsView();
}
