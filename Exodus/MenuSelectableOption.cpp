#include "MenuSelectableOption.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSelectableOption::MenuSelectableOption(IMenuHandler& amenuHandler, int amenuItemID, const std::wstring& aname)
:menuHandler(amenuHandler), menuItemID(amenuItemID), name(aname)
{}

//----------------------------------------------------------------------------------------
//Menu handler functions
//----------------------------------------------------------------------------------------
const wchar_t* MenuSelectableOption::GetNameInternal() const
{
	return name.c_str();
}

//----------------------------------------------------------------------------------------
IMenuHandler& MenuSelectableOption::GetMenuHandler() const
{
	return menuHandler;
}

//----------------------------------------------------------------------------------------
int MenuSelectableOption::GetMenuItemID() const
{
	return menuItemID;
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
IMenuSelectableOption::Type MenuSelectableOption::GetType() const
{
	return TYPE_SELECTABLEOPTION;
}
