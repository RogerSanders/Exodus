#include "MenuSelectableOption.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSelectableOption::MenuSelectableOption(IMenuHandler& amenuHandler, int amenuItemID, const std::wstring& atitle)
:menuHandler(amenuHandler), menuItemID(amenuItemID), title(atitle), physicalMenuHandle(0), physicalMenuItemID(0), checkedState(false)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int MenuSelectableOption::GetIMenuItemVersion() const
{
	return ThisIMenuItemVersion();
}

//----------------------------------------------------------------------------------------
unsigned int MenuSelectableOption::GetIMenuSelectableOptionVersion() const
{
	return ThisIMenuSelectableOptionVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
MenuSelectableOption::Type MenuSelectableOption::GetType() const
{
	return TYPE_SELECTABLEOPTION;
}

//----------------------------------------------------------------------------------------
//Menu title functions
//----------------------------------------------------------------------------------------
std::wstring MenuSelectableOption::GetMenuTitle() const
{
	return title;
}

//----------------------------------------------------------------------------------------
void MenuSelectableOption::GetMenuTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetMenuTitle());
}

//----------------------------------------------------------------------------------------
//Menu handler functions
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
