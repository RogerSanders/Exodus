#include "SystemOptionMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::SystemOptionMenuHandler::SystemOptionMenuHandler(System* adevice)
:device(adevice), nextMenuItemID(0)
{}

//----------------------------------------------------------------------------------------
//System setting functions
//----------------------------------------------------------------------------------------
unsigned int System::SystemOptionMenuHandler::AddSystemSettingMenuItem(unsigned int moduleID, std::wstring settingName, unsigned int settingOptionNo)
{
	//Retrieve the ID number to use for this menu item, and increment the ID number for
	//the next menu item.
	unsigned int menuItemID = nextMenuItemID++;

	//Construct an object representing this menu entry
	SettingMenuEntry menuEntry;
	menuEntry.moduleID = moduleID;
	menuEntry.settingName = settingName;
	menuEntry.optionID = settingOptionNo;

	//Insert this menu entry into the list of menu items
	menuItems.insert(std::pair<unsigned int, SettingMenuEntry>(menuItemID, menuEntry));

	//Return the new menu item ID
	return menuItemID;
}

//----------------------------------------------------------------------------------------
//Menu item handler functions
//----------------------------------------------------------------------------------------
void System::SystemOptionMenuHandler::HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher)
{
	//Retrieve the menu setting associated with this menu item
	std::map<unsigned int, SettingMenuEntry>::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator == menuItems.end())
	{
		return;
	}
	SettingMenuEntry& menuItem = menuItemIterator->second;

	//Retrieve the list of settings associated with the target module
	System::ModuleSystemSettingMap::iterator moduleSettingsMapIterator = device->moduleSettings.find(menuItem.moduleID);
	if(moduleSettingsMapIterator == device->moduleSettings.end())
	{
		return;
	}
	SystemSettingsList& moduleSettingsList = moduleSettingsMapIterator->second;

	//Locate the referenced module setting
	SystemSettingInfo* settingInfo = 0;
	bool settingFound = false;
	System::SystemSettingsList::iterator moduleSettingsIterator = moduleSettingsList.begin();
	while(!settingFound && (moduleSettingsIterator != moduleSettingsList.end()))
	{
		if(moduleSettingsIterator->name == menuItem.settingName)
		{
			settingFound = true;
			settingInfo = &(*moduleSettingsIterator);
		}
		++moduleSettingsIterator;
	}
	if(!settingFound)
	{
		return;
	}

	//Retrieve the info for the target option
	if(menuItem.optionID >= (unsigned int)settingInfo->options.size())
	{
		return;
	}
	const SystemSettingOption& optionInfo = settingInfo->options[menuItem.optionID];

	//Save the current system running state, and stop the system.
	bool systemRunningState = device->SystemRunning();
	device->StopSystem();

	//Replace the current option selection
	if(settingInfo->options[settingInfo->selectedOption].menuItemEntry != 0)
	{
		settingInfo->options[settingInfo->selectedOption].menuItemEntry->SetCheckedState(false);
	}
	settingInfo->selectedOption = menuItem.optionID;
	if(settingInfo->options[settingInfo->selectedOption].menuItemEntry != 0)
	{
		settingInfo->options[settingInfo->selectedOption].menuItemEntry->SetCheckedState(true);
	}

	//Apply all system settings changes listed under this option
	for(std::list<SystemStateChange>::const_iterator i = optionInfo.stateChanges.begin(); i != optionInfo.stateChanges.end(); ++i)
	{
		device->ApplySystemStateChange(*i);
	}

	//Restore the system running state
	if(systemRunningState)
	{
		device->RunSystem();
	}
}
