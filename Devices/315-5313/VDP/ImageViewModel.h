#include "VDP.h"
#ifndef __VDP_IMAGEVIEWMODEL_H__
#define __VDP_IMAGEVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::ImageViewModel :public ViewModelBase
{
public:
	//Constructors
	ImageViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
};

#endif
