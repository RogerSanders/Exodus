#ifndef __IMENUSEPARATOR_H__
#define __IMENUSEPARATOR_H__
#include "IMenuItem.h"

class IMenuSeparator :public IMenuItem
{
public:
	//Interface version functions
	static inline unsigned int ThisIMenuSeparatorVersion();
	virtual unsigned int GetIMenuSeparatorVersion() const = 0;
};

#include "IMenuSeparator.inl"
#endif
