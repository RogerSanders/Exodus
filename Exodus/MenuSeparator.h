#ifndef __MENUSEPARATOR_H__
#define __MENUSEPARATOR_H__
#include "SystemInterface/SystemInterface.pkg"

class MenuSeparator :public IMenuSeparator
{
public:
	//Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSeparatorVersion() const;

	//Type functions
	virtual Type GetType() const;
};

#endif
