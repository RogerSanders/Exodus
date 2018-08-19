#include "ModuleOptionMenuHandler.h"
#include <functional>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ModuleOptionMenuHandler::ModuleOptionMenuHandler(ExodusSystemMenus& owner, ISystemGUIInterface& model, unsigned int moduleID)
:_owner(owner), _model(model), _moduleID(moduleID), _builtModuleSettingsList(false)
{}

//----------------------------------------------------------------------------------------
ModuleOptionMenuHandler::~ModuleOptionMenuHandler()
{
	for(std::map<unsigned int, SettingEntry*>::const_iterator i = _moduleSettings.begin(); i != _moduleSettings.end(); ++i)
	{
		delete i->second;
	}
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ModuleOptionMenuHandler::GetIMenuHandlerVersion() const
{
	return ThisIMenuHandlerVersion();
}

//----------------------------------------------------------------------------------------
//Menu binding functions
//----------------------------------------------------------------------------------------
void ModuleOptionMenuHandler::AddMenuItems(IMenuSegment& menuSegment)
{
	//Build the settings list for the target module if it isn't already built
	if(!_builtModuleSettingsList)
	{
		//Build a list of settings in the target module
		_moduleSettings = BuildModuleSettingsList();

		//Flag that the settings list has now been built
		_builtModuleSettingsList = true;
	}

	//Add menu items for each system option defined in this module
	_menuOptions.clear();
	int nextMenuItemID = 0;
	for(std::map<unsigned int, SettingEntry*>::iterator moduleSettingsIterator = _moduleSettings.begin(); moduleSettingsIterator != _moduleSettings.end(); ++moduleSettingsIterator)
	{
		//Retrieve the index of the currently selected option for the target module
		//setting
		SettingEntry& settingEntry = *moduleSettingsIterator->second;
		unsigned int selectedOptionIndex;
		bool foundSelectedOptionIndex = _model.GetModuleSettingActiveOptionIndex(_moduleID, settingEntry.settingID, selectedOptionIndex);

		//Build the menu for this module setting
		if(settingEntry.toggleSetting)
		{
			//Add a menu item for this toggle setting
			int menuItemID = nextMenuItemID++;
			IMenuSelectableOption& settingMenuItem = menuSegment.AddMenuItemSelectableOption(*this, menuItemID, settingEntry.displayName);
			settingEntry.menuItemEntry = &settingMenuItem;
			settingMenuItem.SetCheckedState(foundSelectedOptionIndex && (selectedOptionIndex == settingEntry.toggleSettingOnOptionIndex));
			_menuOptions[menuItemID] = MenuOption(settingEntry.settingID);
		}
		else
		{
			//Add menu item entries for each option in this setting
			IMenuSubmenu& settingSubmenu = menuSegment.AddMenuItemSubmenu(settingEntry.displayName);
			IMenuSegment& menuSegmentForSettingSubmenu = settingSubmenu.AddMenuItemSegment(false, IMenuSegment::SORTMODE_TITLE);
			for(unsigned int optionIndex = 0; optionIndex < (unsigned int)settingEntry.options.size(); ++optionIndex)
			{
				int menuItemID = nextMenuItemID++;
				OptionEntry& optionEntry = settingEntry.options[optionIndex];
				IMenuSelectableOption& optionMenuItem = menuSegmentForSettingSubmenu.AddMenuItemSelectableOption(*this, menuItemID, optionEntry.displayName);
				optionEntry.menuItemEntry = &optionMenuItem;
				optionMenuItem.SetCheckedState(foundSelectedOptionIndex && (selectedOptionIndex == optionIndex));
				_menuOptions[menuItemID] = MenuOption(settingEntry.settingID, optionIndex);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
std::map<unsigned int, ModuleOptionMenuHandler::SettingEntry*> ModuleOptionMenuHandler::BuildModuleSettingsList()
{
	//Build a list of all settings in the target module
	std::map<unsigned int, SettingEntry*> moduleSettingsList;
	std::list<unsigned int> moduleSettingIDList = _model.GetModuleSettingIDs(_moduleID);
	for(std::list<unsigned int>::const_iterator moduleSettingIterator = moduleSettingIDList.begin(); moduleSettingIterator != moduleSettingIDList.end(); ++moduleSettingIterator)
	{
		//Retrieve info for this module setting
		unsigned int settingID = *moduleSettingIterator;
		ModuleSettingInfo moduleSettingInfo;
		if(!_model.GetModuleSettingInfo(_moduleID, settingID, moduleSettingInfo))
		{
			continue;
		}

		//Build a SettingEntry object for this module setting
		SettingEntry* settingEntry = new SettingEntry(_model, _moduleID, settingID, std::bind(std::mem_fn(&ModuleOptionMenuHandler::RefreshActiveMenuSettingOption), this, settingID));
		settingEntry->displayName = moduleSettingInfo.GetDisplayName();
		settingEntry->toggleSetting = moduleSettingInfo.IsToggleSetting();
		settingEntry->toggleSettingOnOptionIndex = moduleSettingInfo.GetToggleSettingOnOptionIndex();
		settingEntry->toggleSettingOffOptionIndex = moduleSettingInfo.GetToggleSettingOffOptionIndex();

		//If the target module setting isn't a toggle setting, record information on
		//each of its options.
		if(!settingEntry->toggleSetting)
		{
			//Retrieve information on each option for this toggle setting
			unsigned int optionCount = moduleSettingInfo.GetOptionCount();
			for(unsigned int i = 0; i < optionCount; ++i)
			{
				//Retrieve info for this setting option
				ModuleSettingOptionInfo moduleSettingOptionInfo;
				if(!_model.GetModuleSettingOptionInfo(_moduleID, settingID, i, moduleSettingOptionInfo))
				{
					continue;
				}

				//Add information on this option to the setting
				OptionEntry optionEntry;
				optionEntry.displayName = moduleSettingOptionInfo.GetDisplayName();
				optionEntry.optionID = i;
				settingEntry->options.push_back(optionEntry);
			}

			//If no options were found for this toggle setting, skip it.
			if(settingEntry->options.empty())
			{
				delete settingEntry;
				continue;
			}
		}

		//Save information on this module setting
		moduleSettingsList.insert(std::pair<unsigned int, SettingEntry*>(settingID, settingEntry));
	}

	//Return the module settings list to the caller
	return moduleSettingsList;
}

//----------------------------------------------------------------------------------------
void ModuleOptionMenuHandler::RefreshActiveMenuSettingOption(unsigned int settingID)
{
	//Locate the referenced module setting
	std::map<unsigned int, SettingEntry*>::iterator moduleSettingsIterator = _moduleSettings.find(settingID);
	if(moduleSettingsIterator == _moduleSettings.end())
	{
		return;
	}
	SettingEntry& settingEntry = *moduleSettingsIterator->second;

	//Retrieve the index of the currently selected option for the target module setting
	unsigned int selectedOptionIndex;
	if(!_model.GetModuleSettingActiveOptionIndex(_moduleID, settingEntry.settingID, selectedOptionIndex))
	{
		return;
	}

	//Update the checked state of each menu item for this setting
	if(settingEntry.toggleSetting)
	{
		settingEntry.menuItemEntry->SetCheckedState(selectedOptionIndex == settingEntry.toggleSettingOnOptionIndex);
	}
	else
	{
		for(unsigned int optionIndex = 0; optionIndex < (unsigned int)settingEntry.options.size(); ++optionIndex)
		{
			OptionEntry& optionEntry = settingEntry.options[optionIndex];
			optionEntry.menuItemEntry->SetCheckedState(selectedOptionIndex == optionIndex);
		}
	}
}

//----------------------------------------------------------------------------------------
//Menu item handler functions
//----------------------------------------------------------------------------------------
void ModuleOptionMenuHandler::HandleMenuItemSelect(int menuItemID)
{
	//Retrieve the menu setting associated with this menu item
	std::map<int, MenuOption>::const_iterator menuOptionIterator = _menuOptions.find(menuItemID);
	if(menuOptionIterator == _menuOptions.end())
	{
		return;
	}
	const MenuOption& menuOption = menuOptionIterator->second;

	//Locate the referenced module setting
	std::map<unsigned int, SettingEntry*>::const_iterator moduleSettingsIterator = _moduleSettings.find(menuOption.settingID);
	if(moduleSettingsIterator == _moduleSettings.end())
	{
		return;
	}
	const SettingEntry& settingEntry = *moduleSettingsIterator->second;

	//Select the new target option to be applied as a result of this menu item selection
	//event
	unsigned int targetOption;
	if(!settingEntry.toggleSetting)
	{
		targetOption = menuOption.optionID;
	}
	else
	{
		unsigned int activeOptionIndex;
		if(!_model.GetModuleSettingActiveOptionIndex(_moduleID, settingEntry.settingID, activeOptionIndex))
		{
			return;
		}
		targetOption = (activeOptionIndex == settingEntry.toggleSettingOnOptionIndex)? settingEntry.toggleSettingOffOptionIndex: settingEntry.toggleSettingOnOptionIndex;
	}

	//Apply the new target setting option
	_model.SetModuleSettingActiveOptionIndex(_moduleID, settingEntry.settingID, targetOption);
}
