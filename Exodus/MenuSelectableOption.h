#ifndef __MENUSELECTABLEOPTION_H__
#define __MENUSELECTABLEOPTION_H__
#include "ExtensionInterface/ExtensionInterface.pkg"

class MenuSelectableOption :public IMenuSelectableOption
{
public:
	//Constructors
	MenuSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title);

	//Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSelectableOptionVersion() const;

	//Type functions
	virtual Type GetType() const;

	//Menu title functions
	virtual Marshal::Ret<std::wstring> GetMenuTitle() const;

	//Menu handler functions
	virtual IMenuHandler& GetMenuHandler() const;
	virtual int GetMenuItemID() const;

	//Physical menu functions
	HMENU GetPhysicalMenuHandle() const;
	void SetPhysicalMenuHandle(HMENU physicalMenuHandle);
	unsigned int GetPhysicalMenuItemID() const;
	void SetPhysicalMenuItemID(unsigned int physicalMenuItemID);

	//Checked state functions
	virtual bool GetCheckedState() const;
	virtual void SetCheckedState(bool checkedState);

private:
	std::wstring _title;
	IMenuHandler& _menuHandler;
	int _menuItemID;
	HMENU _physicalMenuHandle;
	unsigned int _physicalMenuItemID;
	bool _checkedState;
};

#endif
