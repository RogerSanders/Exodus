#include "ExodusSystemMenus.h"
#include "DebugMenuHandler.h"
#include "SettingsMenuHandler.h"
#include "SystemMenuHandler.h"
#include "ModuleOptionMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExodusSystemMenus::ExodusSystemMenus(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:Extension(aimplementationName, ainstanceName, amoduleID), model(0), debugMenuHandler(0), settingsMenuHandler(0), systemMenuHandler(0)
{}

//----------------------------------------------------------------------------------------
ExodusSystemMenus::~ExodusSystemMenus()
{
	//Delete all menu handlers
	if(debugMenuHandler != 0)
	{
		debugMenuHandler->ClearMenuItems();
		delete debugMenuHandler;
	}
	if(settingsMenuHandler != 0)
	{
		settingsMenuHandler->ClearMenuItems();
		delete settingsMenuHandler;
	}
	if(systemMenuHandler != 0)
	{
		systemMenuHandler->ClearMenuItems();
		delete systemMenuHandler;
	}
	for(std::map<unsigned int, ModuleOptionMenuHandler*>::const_iterator i = moduleOptionMenuHandlers.begin(); i != moduleOptionMenuHandlers.end(); ++i)
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
	model = &((ISystemGUIInterface&)GetSystemInterface());

	//Create all menu handlers
	debugMenuHandler = new DebugMenuHandler(*this, *model);
	debugMenuHandler->LoadMenuItems();
	settingsMenuHandler = new SettingsMenuHandler(*this, *model);
	settingsMenuHandler->LoadMenuItems();
	systemMenuHandler = new SystemMenuHandler(*this, *model);
	systemMenuHandler->LoadMenuItems();

	return true;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::RegisterModuleMenuHandler(unsigned int moduleID)
{
	ModuleOptionMenuHandler* menuHandler = new ModuleOptionMenuHandler(*this, *model, moduleID);
	moduleOptionMenuHandlers.insert(std::pair<unsigned int, ModuleOptionMenuHandler*>(moduleID, menuHandler));
	return true;
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::UnregisterModuleMenuHandler(unsigned int moduleID)
{
	std::map<unsigned int, ModuleOptionMenuHandler*>::iterator moduleOptionMenuHandlerIterator = moduleOptionMenuHandlers.find(moduleID);
	if(moduleOptionMenuHandlerIterator != moduleOptionMenuHandlers.end())
	{
		ModuleOptionMenuHandler* entry = moduleOptionMenuHandlerIterator->second;
		delete entry;
		moduleOptionMenuHandlers.erase(moduleOptionMenuHandlerIterator);
	}
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::AddSystemMenuItems(SystemMenu systemMenu, IMenuSegment& menuSegment)
{
	switch(systemMenu)
	{
	case IExtension::SystemMenu::Debug:
		debugMenuHandler->AddMenuItems(menuSegment);
		break;
	case IExtension::SystemMenu::Settings:
		settingsMenuHandler->AddMenuItems(menuSegment);
		break;
	case IExtension::SystemMenu::System:
		systemMenuHandler->AddMenuItems(menuSegment);
		break;
	}
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::AddModuleMenuItems(ModuleMenu moduleMenu, IMenuSegment& menuSegment, unsigned int moduleID)
{
	if(moduleMenu == IExtension::ModuleMenu::Settings)
	{
		ModuleOptionMenuHandler& menuHandler = *moduleOptionMenuHandlers[moduleID];
		menuHandler.AddMenuItems(menuSegment);
	}
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::RestoreSystemViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState)
{
	bool result = false;
	result |= debugMenuHandler->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
	result |= settingsMenuHandler->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
	result |= systemMenuHandler->RestoreMenuViewOpen(viewGroupName, viewName, viewState);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::RestoreModuleViewState(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID)
{
	//Nothing to do here, since our current module option menu handler has no views.
	return false;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::OpenSystemView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName)
{
	bool result = false;
	result |= debugMenuHandler->OpenView(viewGroupName, viewName);
	result |= settingsMenuHandler->OpenView(viewGroupName, viewName);
	result |= systemMenuHandler->OpenView(viewGroupName, viewName);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusSystemMenus::OpenModuleView(const MarshalSupport::Marshal::In<std::wstring>& viewGroupName, const MarshalSupport::Marshal::In<std::wstring>& viewName, unsigned int moduleID)
{
	//Nothing to do here, since our current module option menu handler has no views.
	return false;
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::OpenInputMappingDetailsView(IDevice* targetDevice)
{
	settingsMenuHandler->OpenInputMappingDetailsView(targetDevice);
}

//----------------------------------------------------------------------------------------
void ExodusSystemMenus::CloseInputMappingDetailsView()
{
	settingsMenuHandler->CloseInputMappingDetailsView();
}
