#include "MenuSeparator.h"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int MenuSeparator::GetIMenuItemVersion() const
{
	return ThisIMenuItemVersion();
}

//----------------------------------------------------------------------------------------
unsigned int MenuSeparator::GetIMenuSeparatorVersion() const
{
	return ThisIMenuSeparatorVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
IMenuSeparator::Type MenuSeparator::GetType() const
{
	return TYPE_SEPARATOR;
}
