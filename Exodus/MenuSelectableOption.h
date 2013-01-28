#ifndef __MENUSELECTABLEOPTION_H__
#define __MENUSELECTABLEOPTION_H__
#include "SystemInterface/SystemInterface.pkg"

class MenuSelectableOption :public IMenuSelectableOption
{
public:
	//Constructors
	MenuSelectableOption(IMenuHandler& amenuHandler, int amenuItemID, const std::wstring& aname);

	//Menu handler functions
	virtual IMenuHandler& GetMenuHandler() const;
	virtual int GetMenuItemID() const;

	//Type functions
	virtual Type GetType() const;

	//Physical menu functions
	HMENU GetPhysicalMenuHandle() const;
	void SetPhysicalMenuHandle(HMENU aphysicalMenuHandle);
	unsigned int GetPhysicalMenuItemID() const;
	void SetPhysicalMenuItemID(unsigned int aphysicalMenuItemID);

	//Checked state functions
	virtual bool GetCheckedState() const;
	virtual void SetCheckedState(bool acheckedState);

protected:
	//Menu handler functions
	virtual const wchar_t* GetNameInternal() const;

private:
	std::wstring name;
	IMenuHandler& menuHandler;
	int menuItemID;
	HMENU physicalMenuHandle;
	unsigned int physicalMenuItemID;
	bool checkedState;
};

#endif
