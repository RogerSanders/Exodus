#include "VDP.h"
#ifndef __VDP_VRAMVIEWMODEL_H__
#define __VDP_VRAMVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::VRAMViewModel :public ViewModelBase
{
public:
	//Constructors
	VRAMViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
};

#endif
