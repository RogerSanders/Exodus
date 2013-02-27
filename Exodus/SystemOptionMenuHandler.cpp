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
unsigned int System::SystemOptionMenuHandler::AddSystemSettingMenuItem(unsigned int settingID, unsigned int settingOptionNo)
{
	//Retrieve the ID number to use for this menu item, and increment the ID number for
	//the next menu item.
	unsigned int menuItemID = nextMenuItemID++;

	//Construct an object representing this menu entry
	SettingMenuEntry menuEntry;
	menuEntry.settingID = settingID;
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
	std::map<unsigned int, SettingMenuEntry>::const_iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator == menuItems.end())
	{
		return;
	}
	const SettingMenuEntry& menuItem = menuItemIterator->second;

	//Locate the referenced module setting
	SystemSettingInfo* settingInfo = 0;
	SystemSettingsMap::iterator systemSettingsIterator = device->systemSettings.find(menuItem.settingID);
	if(systemSettingsIterator == device->systemSettings.end())
	{
		return;
	}
	settingInfo = &(systemSettingsIterator->second);

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
