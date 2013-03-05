#include "VDP.h"
#ifndef __VDP_REGISTERSVIEWMODEL_H__
#define __VDP_REGISTERSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::RegistersViewModel :public ViewModelBase
{
public:
	//Constructors
	RegistersViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
};

#endif
