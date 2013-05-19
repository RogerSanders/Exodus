#include "SystemOptionMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::SystemOptionMenuHandler::SystemOptionMenuHandler(System* adevice)
:device(adevice), nextMenuItemID(0)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int System::SystemOptionMenuHandler::GetIMenuHandlerVersion() const
{
	return ThisIMenuHandlerVersion();
}

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

	//Select the new target option to be applied as a result of this menu item selection
	//event
	unsigned int newTargetOption = menuItem.optionID;
	if(settingInfo->toggleSetting)
	{
		newTargetOption = (settingInfo->selectedOption == settingInfo->onOption)? settingInfo->offOption: settingInfo->onOption;
	}

	//Retrieve the info for the target option
	if(newTargetOption >= (unsigned int)settingInfo->options.size())
	{
		return;
	}
	const SystemSettingOption& optionInfo = settingInfo->options[newTargetOption];

	//Save the current system running state, and stop the system.
	bool systemRunningState = device->SystemRunning();
	device->StopSystem();

	//Replace the current option selection
	if(settingInfo->toggleSetting)
	{
		if(!settingInfo->toggleSettingAutoRevert)
		{
			settingInfo->selectedOption = newTargetOption;
			if(settingInfo->menuItemEntry != 0)
			{
				settingInfo->menuItemEntry->SetCheckedState((settingInfo->selectedOption == settingInfo->onOption));
			}
		}
	}
	else
	{
		if(settingInfo->options[settingInfo->selectedOption].menuItemEntry != 0)
		{
			settingInfo->options[settingInfo->selectedOption].menuItemEntry->SetCheckedState(false);
		}
		settingInfo->selectedOption = newTargetOption;
		if(settingInfo->options[settingInfo->selectedOption].menuItemEntry != 0)
		{
			settingInfo->options[settingInfo->selectedOption].menuItemEntry->SetCheckedState(true);
		}
	}

	//If requested, execute a lead-in time before applying the setting change.
	if(settingInfo->settingChangeLeadInTime > 0)
	{
		//Calculate the time to execute until the setting should be changed. If a random
		//time has been requested, we generate a random number between the specified
		//minimum and maximum time constraints.
		double timeTillSettingChange = settingInfo->settingChangeLeadInTime;
		if(settingInfo->settingChangeLeadInTimeRandom)
		{
			timeTillSettingChange = (double)rand() * ((settingInfo->settingChangeLeadInTimeEnd - settingInfo->settingChangeLeadInTime) / (double)RAND_MAX);
		}

		//Execute the system up to the setting change time
		device->ExecuteSystemStepManual(timeTillSettingChange);
	}

	//Apply all system settings changes listed under this option
	for(std::list<SystemStateChange>::const_iterator i = optionInfo.stateChanges.begin(); i != optionInfo.stateChanges.end(); ++i)
	{
		device->ApplySystemStateChange(*i);
	}

	//If this is an auto-reverting toggle option, advance until the required time, and
	//revert to the previous setting.
	if(settingInfo->toggleSettingAutoRevert)
	{
		//Calculate the time to execute until the setting should be reverted. If a random
		//time has been requested, we generate a random number between the specified
		//minimum and maximum time constraints.
		double timeTillAutoRevert = settingInfo->toggleSettingAutoRevertTime;
		if(settingInfo->toggleSettingAutoRevertTimeRandom)
		{
			timeTillAutoRevert = (double)rand() * ((settingInfo->toggleSettingAutoRevertTimeEnd - settingInfo->toggleSettingAutoRevertTime) / (double)RAND_MAX);
		}

		//Execute the system up to the revert time
		device->ExecuteSystemStepManual(timeTillAutoRevert);

		//Obtain info on the reverted option
		unsigned int revertTargetOption = settingInfo->selectedOption;
		const SystemSettingOption& revertOptionInfo = settingInfo->options[revertTargetOption];

		//Apply all settings to revert to the previous option state
		for(std::list<SystemStateChange>::const_iterator i = revertOptionInfo.stateChanges.begin(); i != revertOptionInfo.stateChanges.end(); ++i)
		{
			device->ApplySystemStateChange(*i);
		}
	}

	//Restore the system running state
	if(systemRunningState)
	{
		device->RunSystem();
	}
}
