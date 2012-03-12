#include "VDP.h"
#ifndef __VDP_PORTMONITORVIEWMODEL_H__
#define __VDP_PORTMONITORVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::PortMonitorViewModel :public ViewModelBase
{
public:
	//Constructors
	PortMonitorViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
};

#endif
