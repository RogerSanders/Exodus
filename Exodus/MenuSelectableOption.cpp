#include "MenuSelectableOption.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSelectableOption::MenuSelectableOption(IMenuHandler& amenuHandler, int amenuItemID, const std::wstring& aname)
:menuHandler(amenuHandler), menuItemID(amenuItemID), name(aname), physicalMenuHandle(0), physicalMenuItemID(0), checkedState(false)
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

//----------------------------------------------------------------------------------------
//Physical menu functions
//----------------------------------------------------------------------------------------
HMENU MenuSelectableOption::GetPhysicalMenuHandle() const
{
	return physicalMenuHandle;
}

//----------------------------------------------------------------------------------------
void MenuSelectableOption::SetPhysicalMenuHandle(HMENU aphysicalMenuHandle)
{
	physicalMenuHandle = aphysicalMenuHandle;
}

//----------------------------------------------------------------------------------------
unsigned int MenuSelectableOption::GetPhysicalMenuItemID() const
{
	return physicalMenuItemID;
}

//----------------------------------------------------------------------------------------
void MenuSelectableOption::SetPhysicalMenuItemID(unsigned int aphysicalMenuItemID)
{
	physicalMenuItemID = aphysicalMenuItemID;
}

//----------------------------------------------------------------------------------------
//Checked state functions
//----------------------------------------------------------------------------------------
bool MenuSelectableOption::GetCheckedState() const
{
	return checkedState;
}

//----------------------------------------------------------------------------------------
void MenuSelectableOption::SetCheckedState(bool acheckedState)
{
	checkedState = acheckedState;

	//Change the checked state on the physical menu item
	if((physicalMenuHandle != 0) && (physicalMenuItemID != 0))
	{
		CheckMenuItem(physicalMenuHandle, (UINT)physicalMenuItemID, checkedState? MF_CHECKED: MF_UNCHECKED);
	}
}
