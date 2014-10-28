#ifndef __IMENUSELECTABLEOPTION_H__
#define __IMENUSELECTABLEOPTION_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include "IMenuItem.h"
#include "IMenuHandler.h"
#include <string>

class IMenuSelectableOption :public IMenuItem
{
public:
	//Interface version functions
	static inline unsigned int ThisIMenuSelectableOptionVersion() { return 1; }
	virtual unsigned int GetIMenuSelectableOptionVersion() const = 0;

	//Menu title functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetMenuTitle() const = 0;

	//Menu handler functions
	virtual IMenuHandler& GetMenuHandler() const = 0;
	virtual int GetMenuItemID() const = 0;

	//Checked state functions
	virtual bool GetCheckedState() const = 0;
	virtual void SetCheckedState(bool acheckedState) = 0;

	//##TODO## Add support for icons
};

#endif
