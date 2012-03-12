#include "SN76489.h"
#ifndef __SN76489_PROPERTIESVIEWMODEL_H__
#define __SN76489_PROPERTIESVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class SN76489::PropertiesViewModel :public ViewModelBase
{
public:
	//Constructors
	PropertiesViewModel(const std::wstring& amenuHandlerName, int aviewModelID, SN76489* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	SN76489* device;
};

#endif
