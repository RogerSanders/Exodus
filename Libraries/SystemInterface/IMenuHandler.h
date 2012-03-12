#ifndef __IMENUHANDLER_H__
#define __IMENUHANDLER_H__
#include "IViewModelLauncher.h"

class IMenuHandler
{
public:
	//Constructors
	virtual ~IMenuHandler() = 0 {}

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher) = 0;
};

#endif
