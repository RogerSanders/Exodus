#include "MenuSelectableOption.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSelectableOption::MenuSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title)
:_menuHandler(menuHandler), _menuItemID(menuItemID), _title(title), _physicalMenuHandle(0), _physicalMenuItemID(0), _checkedState(false)
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
	return Type::SelectableOption;
}

//----------------------------------------------------------------------------------------
//Menu title functions
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> MenuSelectableOption::GetMenuTitle() const
{
	return _title;
}

//----------------------------------------------------------------------------------------
//Menu handler functions
//----------------------------------------------------------------------------------------
IMenuHandler& MenuSelectableOption::GetMenuHandler() const
{
	return _menuHandler;
}

//----------------------------------------------------------------------------------------
int MenuSelectableOption::GetMenuItemID() const
{
	return _menuItemID;
}

//----------------------------------------------------------------------------------------
//Physical menu functions
//----------------------------------------------------------------------------------------
HMENU MenuSelectableOption::GetPhysicalMenuHandle() const
{
	return _physicalMenuHandle;
}

//----------------------------------------------------------------------------------------
void MenuSelectableOption::SetPhysicalMenuHandle(HMENU physicalMenuHandle)
{
	_physicalMenuHandle = physicalMenuHandle;
}

//----------------------------------------------------------------------------------------
unsigned int MenuSelectableOption::GetPhysicalMenuItemID() const
{
	return _physicalMenuItemID;
}

//----------------------------------------------------------------------------------------
void MenuSelectableOption::SetPhysicalMenuItemID(unsigned int physicalMenuItemID)
{
	_physicalMenuItemID = physicalMenuItemID;
}

//----------------------------------------------------------------------------------------
//Checked state functions
//----------------------------------------------------------------------------------------
bool MenuSelectableOption::GetCheckedState() const
{
	return _checkedState;
}

//----------------------------------------------------------------------------------------
void MenuSelectableOption::SetCheckedState(bool checkedState)
{
	_checkedState = checkedState;

	//Change the checked state on the physical menu item
	if ((_physicalMenuHandle != 0) && (_physicalMenuItemID != 0))
	{
		CheckMenuItem(_physicalMenuHandle, (UINT)_physicalMenuItemID, _checkedState? MF_CHECKED: MF_UNCHECKED);
	}
}
