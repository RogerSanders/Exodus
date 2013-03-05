#include "S315_5313.h"
#ifndef __S315_5313_PORTMONITORDETAILSVIEWMODEL_H__
#define __S315_5313_PORTMONITORDETAILSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::PortMonitorDetailsViewModel :public ViewModelBase
{
public:
	//Constructors
	PortMonitorDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice, const PortMonitorEntry& aentry);

	//Port monitor entry functions
	void SetPortMonitorEntry(const PortMonitorEntry& aentry);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	S315_5313* device;
	PortMonitorEntry entry;
};

#endif
