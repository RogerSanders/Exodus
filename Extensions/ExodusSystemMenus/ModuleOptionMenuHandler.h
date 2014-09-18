#ifndef __MODULEOPTIONMENUHANDLER_H__
#define __MODULEOPTIONMENUHANDLER_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "ExodusSystemMenus.h"
#include "SystemInterface/SystemInterface.pkg"
#include <map>

class ModuleOptionMenuHandler :public IMenuHandler
{
public:
	//Constructors
	ModuleOptionMenuHandler(ExodusSystemMenus& aowner, ISystemGUIInterface& amodel, unsigned int amoduleID);
	~ModuleOptionMenuHandler();

	//Interface version functions
	virtual unsigned int GetIMenuHandlerVersion() const;

	//Menu binding functions
	void AddMenuItems(IMenuSegment& menuSegment);

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID);

private:
	//Structures
	struct MenuOption
	{
		MenuOption(unsigned int asettingID = 0, unsigned int aoptionID = 0)
		:settingID(asettingID), optionID(aoptionID)
		{}

		unsigned int settingID;
		unsigned int optionID;
	};
	struct OptionEntry
	{
		unsigned int optionID;
		IMenuSelectableOption* menuItemEntry;
		std::wstring displayName;
	};
	struct SettingEntry
	{
		SettingEntry(const boost::function<void()>& acallback)
		:optionChangeObserver(acallback)
		{}

		bool toggleSetting;
		unsigned int toggleSettingOnOptionIndex;
		unsigned int toggleSettingOffOptionIndex;
		unsigned int settingID;
		std::vector<OptionEntry> options;
		IMenuSelectableOption* menuItemEntry;
		std::wstring displayName;
		ObserverSubscription optionChangeObserver;
	};

private:
	//Menu binding functions
	std::map<unsigned int, SettingEntry*> BuildModuleSettingsList();
	void RefreshActiveMenuSettingOption(unsigned int settingID);

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
	unsigned int moduleID;
	std::map<unsigned int, SettingEntry*> moduleSettings;
	std::map<int, MenuOption> menuOptions;
	bool builtModuleSettingsList;
};

#endif
