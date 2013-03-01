#ifndef __IMENUHANDLER_H__
#define __IMENUHANDLER_H__
#include "IViewModelLauncher.h"

class IMenuHandler
{
public:
	//Constructors
	virtual ~IMenuHandler() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIMenuHandlerVersion();
	virtual unsigned int GetIMenuHandlerVersion() const = 0;

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher) = 0;
};

#include "IMenuHandler.inl"
#endif
