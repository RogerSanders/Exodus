#include "VDP.h"
#ifndef __VDP_PALETTEVIEWMODEL_H__
#define __VDP_PALETTEVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::PaletteViewModel :public ViewModelBase
{
public:
	//Constructors
	PaletteViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
};

#endif
