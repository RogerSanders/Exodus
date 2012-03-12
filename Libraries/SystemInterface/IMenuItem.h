#ifndef __IMENUITEM_H__
#define __IMENUITEM_H__

class IMenuItem
{
public:
	//Enumerations
	enum Type;

	//Constructors
	virtual ~IMenuItem() = 0 {}

	//Type functions
	virtual Type GetType() const = 0;
};

#include "IMenuItem.inl"
#endif
