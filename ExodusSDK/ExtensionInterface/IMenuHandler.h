#ifndef __IMENUHANDLER_H__
#define __IMENUHANDLER_H__

class IMenuHandler
{
public:
	//Constructors
	virtual ~IMenuHandler() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIMenuHandlerVersion() { return 1; }
	virtual unsigned int GetIMenuHandlerVersion() const = 0;

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID) = 0;
};

#endif
