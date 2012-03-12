#ifndef __MENUSEPARATOR_H__
#define __MENUSEPARATOR_H__
#include "SystemInterface/SystemInterface.pkg"

class MenuSeparator :public IMenuSeparator
{
public:
	//Type functions
	virtual Type GetType() const;
};

#endif
