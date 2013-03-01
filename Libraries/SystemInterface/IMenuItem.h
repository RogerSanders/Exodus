#ifndef __IMENUITEM_H__
#define __IMENUITEM_H__

class IMenuItem
{
public:
	//Enumerations
	enum Type;

public:
	//Constructors
	virtual ~IMenuItem() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIMenuItemVersion();
	virtual unsigned int GetIMenuItemVersion() const = 0;

	//Type functions
	virtual Type GetType() const = 0;
};

#include "IMenuItem.inl"
#endif
