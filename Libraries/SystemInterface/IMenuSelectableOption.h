#ifndef __IMENUSELECTABLEOPTION_H__
#define __IMENUSELECTABLEOPTION_H__
#include "IMenuItem.h"
#include "IMenuHandler.h"
#include <string>

class IMenuSelectableOption :public IMenuItem
{
public:
	//Menu handler functions
	inline std::wstring GetName() const;
	virtual IMenuHandler& GetMenuHandler() const = 0;
	virtual int GetMenuItemID() const = 0;

protected:
	//Menu handler functions
	virtual const wchar_t* GetNameInternal() const = 0;
};

#include "IMenuSelectableOption.inl"
#endif
