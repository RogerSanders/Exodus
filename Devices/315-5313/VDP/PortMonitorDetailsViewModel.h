#include "VDP.h"
#ifndef __VDP_PORTMONITORDETAILSVIEWMODEL_H__
#define __VDP_PORTMONITORDETAILSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::PortMonitorDetailsViewModel :public ViewModelBase
{
public:
	//Constructors
	PortMonitorDetailsViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice, const PortMonitorEntry& aentry);

	//Port monitor entry functions
	void SetPortMonitorEntry(const PortMonitorEntry& aentry);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
	PortMonitorEntry entry;
};

#endif
