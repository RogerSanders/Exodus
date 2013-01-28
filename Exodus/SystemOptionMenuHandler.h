#include "System.h"
#ifndef __SYSTEM_SYSTEMOPTIONMENUHANDLER_H__
#define __SYSTEM_SYSTEMOPTIONMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"
#include <map>

class System::SystemOptionMenuHandler :public IMenuHandler
{
public:
	//Constructors
	SystemOptionMenuHandler(System* adevice);

	//System setting functions
	unsigned int AddSystemSettingMenuItem(unsigned int moduleID, std::wstring settingName, unsigned int settingOptionNo);

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher);

private:
	//Structures
	struct SettingMenuEntry
	{
		unsigned int moduleID;
		std::wstring settingName;
		unsigned int optionID;
	};

private:
	System* device;
	unsigned int moduleID;
	std::map<unsigned int, SettingMenuEntry> menuItems;
	unsigned int nextMenuItemID;
};

#endif
