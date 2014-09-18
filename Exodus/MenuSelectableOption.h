#ifndef __MENUSELECTABLEOPTION_H__
#define __MENUSELECTABLEOPTION_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"

class MenuSelectableOption :public IMenuSelectableOption
{
public:
	//Constructors
	MenuSelectableOption(IMenuHandler& amenuHandler, int amenuItemID, const std::wstring& atitle);

	//Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSelectableOptionVersion() const;

	//Type functions
	virtual Type GetType() const;

	//Menu title functions
	std::wstring GetMenuTitle() const;

	//Menu handler functions
	virtual IMenuHandler& GetMenuHandler() const;
	virtual int GetMenuItemID() const;

	//Physical menu functions
	HMENU GetPhysicalMenuHandle() const;
	void SetPhysicalMenuHandle(HMENU aphysicalMenuHandle);
	unsigned int GetPhysicalMenuItemID() const;
	void SetPhysicalMenuItemID(unsigned int aphysicalMenuItemID);

	//Checked state functions
	virtual bool GetCheckedState() const;
	virtual void SetCheckedState(bool acheckedState);

protected:
	//Menu title functions
	virtual void GetMenuTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

private:
	std::wstring title;
	IMenuHandler& menuHandler;
	int menuItemID;
	HMENU physicalMenuHandle;
	unsigned int physicalMenuItemID;
	bool checkedState;
};

#endif
